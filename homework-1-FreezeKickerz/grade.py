#!/usr/bin/env python3


import os
import shutil
import subprocess
import signal
import math
import re

# Location of sandbox
sandbox_dir = "sandbox"

# Location of original files and the files to copy
original_dir = "."
files_to_copy = ["zla_test.c",
                "zla.c",
                "zla.h",
                "kobject_test.c",
                "kobject.c",
                "kobject.h",
                "Makefile",
                "linked_list.c",
                "linked_list_test.c",
                "linked_list.h",
                "polymorphic_ints.c",
                "polymorphic_ints.h",
                "polymorphic_ints_test.c"]

# Handin files
handin_files = ["zla_test.c",
                "zla.c",
                "zla.h",
                "kobject_test.c",
                "kobject.c",
                "kobject.h",
                "linked_list.c",
                "linked_list_test.c",
                "linked_list.h",
                "polymorphic_ints.c",
                "polymorphic_ints.h",
                "polymorphic_ints_test.c"]

# Location of traces
traces_dir = "traces"
linked_list_dir = "linked_list_output"
zla_dir = "zla_output"


def specify_input_output_files(arg):
    executable = arg
    if arg == "zla":
        config = [f"./{executable}",zla_dir+"/out", zla_dir+"/"+"out_expected"]
    elif arg == "linked_list":
        config = [f"./{executable}",linked_list_dir+"/out_forward", linked_list_dir+"/"+"forward_expected", linked_list_dir+"/out_backward", linked_list_dir+"/"+"backward_expected"]
    else:
        config = [f"./{executable}"]
    return config

def print_success(test):
    print(f"****SUCCESS: {test}****")

def print_failed(test):
    print(f"****FAILED: {test}****")

def make_assignment(name):
    args = ["make", "clean", name]
    try:
        subprocess.check_output(args, stderr=subprocess.STDOUT)
        return True
    except subprocess.CalledProcessError as e:
        print_failed(name)
        print(e.output.decode())
    except KeyboardInterrupt:
        print_failed(name)
        print("User interrupted compilation")
    except:
        print_failed(name)
        print("Unknown error occurred")
    return False

def grade_output(output):
    # line to parse
    output_line = "Test Cases Passed: "
    # Split the content into lines and look for the target line
    for line in output.splitlines():
        if output_line in line:
            # Extract the numbers using a regex
            match = re.search(r"Test Cases Passed: (\d+)/(\d+)", line)
            if match:
                tests_passed = int(match.group(1))  # First number
                total_tests = int(match.group(2))  # Second number
                print(f"Tests Passed: {tests_passed}, Total Tests: {total_tests}")
                return tests_passed, total_tests
            else:
                print("Pattern not found in the matching line.")
            return 0, 0
            break
    else:
        print("No line with 'Test cases passed: ' found.")
        return 0, 0


def grade_output_kobject(test_output, expected_output):
    try:
        # Run diff and capture its output
        diff_output = subprocess.check_output(
            ["diff", test_output, expected_output],
            stderr=subprocess.STDOUT  # Redirect stderr to stdout
        ).decode()
        # If no differences, output will be empty
        line_count = len(diff_output.splitlines())
        kobject_score = "1/1"
        # print(f"Files are identical. Number of lines in diff output: {line_count}")
        return kobject_score
    except subprocess.CalledProcessError as e:
        # Diff found differences (exit code 1)
        diff_output = e.output.decode()  # Capture the diff output
        line_count = len(diff_output.splitlines())
        kobject_score = "0/1"
        print(f"FAILURE: kobject: Files differ. Number of lines in diff output: {line_count}")
        return kobject_score
    except Exception as ex:
        print(f"FAILURE: kobject: An unexpected error occurred: {ex}")
        return "0/1"

def grade(submission_dir):
    score = {}

    # Clear previous sandbox
    if os.path.exists(sandbox_dir):
        subprocess.check_output(["rm", "-rf", sandbox_dir], stderr=subprocess.STDOUT)
    os.makedirs(sandbox_dir)

    # Copy traces directory
    os.makedirs(os.path.join(sandbox_dir, traces_dir))
    for f in os.listdir(os.path.join(original_dir, traces_dir)):
        shutil.copy2(os.path.join(original_dir, traces_dir, f), os.path.join(sandbox_dir, traces_dir, f))
    
    # Copy zla_output directory
    os.makedirs(os.path.join(sandbox_dir, zla_dir))
    for f in os.listdir(os.path.join(original_dir, zla_dir)):
        shutil.copy2(os.path.join(original_dir, zla_dir, f), os.path.join(sandbox_dir, zla_dir, f))

     # Copy linked_list_output directory
    os.makedirs(os.path.join(sandbox_dir, linked_list_dir))
    for f in os.listdir(os.path.join(original_dir, linked_list_dir)):
        shutil.copy2(os.path.join(original_dir, linked_list_dir, f), os.path.join(sandbox_dir, linked_list_dir, f))

    # Copy handin files
    for handin_file in handin_files:
        shutil.copy2(os.path.join(submission_dir, handin_file), os.path.join(sandbox_dir, handin_file))

    # Copy original files
    for f in files_to_copy:
        if os.path.exists(os.path.join(sandbox_dir, f)):
            os.remove(os.path.join(sandbox_dir, f))
        shutil.copy2(os.path.join(original_dir, f), os.path.join(sandbox_dir, f))

     # Switch to sandbox
    cwd = os.getcwd()
    os.chdir(sandbox_dir)

    # Test kobject
    if make_assignment("kobject"):
        kobject_output_file = "traces/test_output"
        kobject_expected_output_file = "traces/expected_output"
        with open(kobject_output_file, "w") as outfile:
            subprocess.run(["./kobject"], stderr=subprocess.STDOUT, stdout=outfile)
        score["kobject"] = grade_output_kobject(kobject_output_file, kobject_expected_output_file)
         
    # Test other files   
    args = ["zla", "polymorphic_ints", "linked_list"]
    for var in args:
        print(var, ":")
        if make_assignment(var):
            grade = 0
            # Run the executable
            config = specify_input_output_files(var)
            try:
                # result = subprocess.run([f"./{executable}"], check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                result = subprocess.run(config, check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                
                grade = grade_output(result.stdout.decode("utf-8"))
            except Exception as e:
                print_failed(var + ": "+str(e))
            score[var] = str(str(grade[0])+"/"+str(grade[1]))

    # Switch back to original directory
    os.chdir(cwd)

    # Clear previous sandbox
    if os.path.exists(sandbox_dir):
        subprocess.check_output(["rm", "-rf", sandbox_dir], stderr=subprocess.STDOUT)


    return score
        


if __name__ == "__main__":
    submission_dir = "."
    score = grade(submission_dir)
    print("Test cases passed:", score)