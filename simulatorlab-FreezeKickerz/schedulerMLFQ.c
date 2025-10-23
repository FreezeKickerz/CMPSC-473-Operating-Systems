#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

#define MAX_MLFQ_LEVELS 10

typedef struct mlfq_job {
    job_t* job;
    int level;
} mlfq_job_t;

static int always_less(void* d1, void* d2) { return -1; }

// Scheduler state
typedef struct {
    list_t* queues[MAX_MLFQ_LEVELS];
    mlfq_job_t* running;
    uint64_t last_time;
} scheduler_MLFQ_t;

// Forward
static void schedule_next(scheduler_t* sched, scheduler_MLFQ_t* info, uint64_t now) {
    // find highest non empty queue
    mlfq_job_t* next = NULL;
    for (int lvl = 0; lvl < MAX_MLFQ_LEVELS; lvl++) {
        if (list_count(info->queues[lvl]) > 0) {
            list_node_t* node = list_head(info->queues[lvl]);
            next = list_data(node);
            list_remove(info->queues[lvl], node);
            break;
        }
    }
    info->running = next;
    if (next) {
        info->last_time = now;
        uint64_t rem = jobGetRemainingTime(next->job);
        uint64_t run = rem < 1 ? rem : 1;
        schedulerScheduleNextCompletion(sched, now + run);
    }
}

// create
void* schedulerMLFQCreate() {
    scheduler_MLFQ_t* info = malloc(sizeof(*info));
    if (!info) return NULL;
    for (int i = 0; i < MAX_MLFQ_LEVELS; i++) {
        info->queues[i] = list_create(always_less);
    }
    info->running = NULL;
    info->last_time = 0;
    return info;
}

// destroy
void schedulerMLFQDestroy(void* si) {
    scheduler_MLFQ_t* info = si;
    // free running wrapper
    if (info->running) free(info->running);
    // free queued wrappers
    for (int i = 0; i < MAX_MLFQ_LEVELS; i++) {
        list_node_t* n = list_head(info->queues[i]);
        while (n != list_end(info->queues[i])) {
            mlfq_job_t* w = list_data(n);
            list_node_t* nxt = list_next(n);
            free(w);
            n = nxt;
        }
        list_destroy(info->queues[i]);
    }
    free(info);
}

// arrival
void schedulerMLFQScheduleJob(void* si, scheduler_t* sched, job_t* job, uint64_t now) {
    scheduler_MLFQ_t* info = si;
    // wrap new job at priority 0
    mlfq_job_t* nw = malloc(sizeof(*nw));
    nw->job = job;
    nw->level = 0;
    list_insert(info->queues[0], nw);
    
    if (!info->running) {
        // no current just schedule
        schedule_next(sched, info, now);
        return;
    }
    if (0 < info->running->level) {
        // cancel pending
        schedulerCancelNextCompletion(sched);
        // compute run so far
        uint64_t run = now - info->last_time;
        if (run > 0) {
            uint64_t rem = jobGetRemainingTime(info->running->job);
            if (run > rem) run = rem;
            jobSetRemainingTime(info->running->job, rem - run);
            // demote
            if (run >= 1 && info->running->level < MAX_MLFQ_LEVELS - 1) {
                info->running->level++;
            }
        }
        // requeue running
        list_insert(info->queues[info->running->level], info->running);
        info->running = NULL;
        // schedule next 
        schedule_next(sched, info, now);
    }
    // else same priority
}

// timer event
job_t* schedulerMLFQCompleteJob(void* si, scheduler_t* sched, uint64_t now) {
    scheduler_MLFQ_t* info = si;
    mlfq_job_t* w = info->running;
    info->running = NULL;
    job_t* finished = NULL;
    if (w) {
        uint64_t rem = jobGetRemainingTime(w->job);
        if (rem > 1) {
            jobSetRemainingTime(w->job, rem - 1);
            if (w->level < MAX_MLFQ_LEVELS - 1) w->level++;
            list_insert(info->queues[w->level], w);
        } else {
            // job done
            jobSetRemainingTime(w->job, 0);
            finished = w->job;
            free(w);
        }
    }
    // schedule next
    schedule_next(sched, info, now);
    return finished;
}
