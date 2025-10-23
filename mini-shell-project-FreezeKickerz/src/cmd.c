// SPDX-License-Identifier: BSD-3-Clause

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cmd.h"
#include "utils.h"

#define READ  0
#define WRITE 1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
    // change current working directory
    int result = chdir(get_word(dir));
    return result;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
    // terminate the shell
    exit(SHELL_EXIT);
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *parent)
{
    // fork a child to execute the simple command
    pid_t child_pid = fork();
    if (child_pid < 0) {
        parse_error("Fork failed", level);
        return -1;
    }

    if (child_pid == 0) {
        // child set up redirections
        char *infile  = get_word(s->in);
        char *outfile = get_word(s->out);
        char *errfile = get_word(s->err);

        if (infile) {
            int fd = open(infile, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // if stdout and stderr go to the same file
        if (outfile && errfile && strcmp(outfile, errfile) == 0) {
            int flags = (s->io_flags == IO_OUT_APPEND)
                        ? (O_WRONLY | O_CREAT | O_APPEND)
                        : (O_WRONLY | O_CREAT | O_TRUNC);
            int fd = open(outfile, flags, 0644);
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        } else {
            if (outfile) {
                int flags = (s->io_flags == IO_OUT_APPEND)
                            ? (O_WRONLY | O_CREAT | O_APPEND)
                            : (O_WRONLY | O_CREAT | O_TRUNC);
                int fd = open(outfile, flags, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            if (errfile) {
                int flags = (s->io_flags == IO_ERR_APPEND)
                            ? (O_WRONLY | O_CREAT | O_APPEND)
                            : (O_WRONLY | O_CREAT | O_TRUNC);
                int fd = open(errfile, flags, 0644);
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
        }

        // free duplicated strings
        free(infile);
        free(outfile);
        free(errfile);

        // build argv and exec
        int   argc = 0;
        char **argv = get_argv(s, &argc);
        execvp(argv[0], argv);

        // exec failed
        fprintf(stderr, "Execution failed for '%s'\n", get_word(s->verb));
        exit(EXIT_FAILURE);
    }

    // parent wait for child and return its exit code
    int status;
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return -1;
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
                            command_t *parent)
{
    // fork first child
    pid_t pid1 = fork();
    if (pid1 < 0) {
        parse_error("Fork failed", level);
        return false;
    }
    if (pid1 == 0) {
        // execute first command
        exit(parse_command(cmd1, level + 1, parent));
    }

    // fork second child
    pid_t pid2 = fork();
    if (pid2 < 0) {
        parse_error("Fork failed", level);
        return false;
    }
    if (pid2 == 0) {
        // execute second command
        exit(parse_command(cmd2, level + 1, parent));
    }

    // parent wait for both
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    return false;
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
                        command_t *parent)
{
    // create pipe
    int pipefd[2];
    if (pipe(pipefd) < 0)
        parse_error("Pipe failed", level);

    // first child writes to pipe
    pid_t pid1 = fork();
    if (pid1 < 0) {
        parse_error("Fork failed", level);
        return false;
    }
    if (pid1 == 0) {
        close(pipefd[READ]);
        dup2(pipefd[WRITE], STDOUT_FILENO);
        close(pipefd[WRITE]);
        exit(parse_command(cmd1, level + 1, parent));
    }

    // second child reads from pipe
    pid_t pid2 = fork();
    if (pid2 < 0) {
        parse_error("Fork failed", level);
        return false;
    }
    if (pid2 == 0) {
        close(pipefd[WRITE]);
        dup2(pipefd[READ], STDIN_FILENO);
        close(pipefd[READ]);
        exit(parse_command(cmd2, level + 1, parent));
    }

    // parent closes both ends and waits
    close(pipefd[READ]);
    close(pipefd[WRITE]);
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    return false;
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *parent)
{
    int status = 0;

    if (c->op == OP_NONE) {
        // get the verb of the simple command
        char *word = get_word(c->scmd->verb);

        if (strchr(word, '=')) {
            const char *name  = c->scmd->verb->string;
            char       *value = get_word(c->scmd->verb->next_part->next_part);
            status = setenv(name, value, 1);
            return status;
        }
        if (strcmp(c->scmd->verb->string, "cd") == 0) {
            pid_t pid = fork();
            if (pid < 0) {
                parse_error("Fork failed", level);
                return -1;
            }
            if (pid == 0) {
                // inline redirections for cd
                char *infile  = get_word(c->scmd->in);
                char *outfile = get_word(c->scmd->out);
                char *errfile = get_word(c->scmd->err);
                if (infile) {
                    int fd = open(infile, O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (outfile && errfile && strcmp(outfile, errfile) == 0) {
                    int flags = (c->scmd->io_flags == IO_OUT_APPEND)
                                ? (O_WRONLY | O_CREAT | O_APPEND)
                                : (O_WRONLY | O_CREAT | O_TRUNC);
                    int fd = open(outfile, flags, 0644);
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                } else {
                    if (outfile) {
                        int flags = (c->scmd->io_flags == IO_OUT_APPEND)
                                    ? (O_WRONLY | O_CREAT | O_APPEND)
                                    : (O_WRONLY | O_CREAT | O_TRUNC);
                        int fd = open(outfile, flags, 0644);
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                    if (errfile) {
                        int flags = (c->scmd->io_flags == IO_ERR_APPEND)
                                    ? (O_WRONLY | O_CREAT | O_APPEND)
                                    : (O_WRONLY | O_CREAT | O_TRUNC);
                        int fd = open(errfile, flags, 0644);
                        dup2(fd, STDERR_FILENO);
                        close(fd);
                    }
                }
                free(infile);
                free(outfile);
                free(errfile);

                int rc = shell_cd(c->scmd->params);
                if (rc == -1)
                    parse_error("Path not found", level);
                exit(0);
            }
            waitpid(pid, &status, 0);
            // update parent process directory
            status = shell_cd(c->scmd->params);
            return status;
        }

        // built-in exit/quit
        if (strcmp(c->scmd->verb->string, "exit") == 0 ||
            strcmp(c->scmd->verb->string, "quit") == 0)
        {
            return shell_exit();
        }

        // fallback to simple external command
        return parse_simple(c->scmd, level, parent);
    }

    // compound operators
    switch (c->op) {
        case OP_SEQUENTIAL:
            // execute sequentially
            status = parse_command(c->cmd1, level + 1, c);
            status = parse_command(c->cmd2, level + 1, c);
            break;

        case OP_PARALLEL:
            status = run_in_parallel(c->cmd1, c->cmd2, level, c);
            break;

        case OP_CONDITIONAL_NZERO:
            status = parse_command(c->cmd1, level + 1, c);
            if (status != 0)
                status = parse_command(c->cmd2, level + 1, c);
            break;

        case OP_CONDITIONAL_ZERO:
            status = parse_command(c->cmd1, level + 1, c);
            if (status == 0)
                status = parse_command(c->cmd2, level + 1, c);
            break;

        case OP_PIPE:
            status = run_on_pipe(c->cmd1, c->cmd2, level, parent);
            break;

        default:
            return SHELL_EXIT;
    }

    return status;
}