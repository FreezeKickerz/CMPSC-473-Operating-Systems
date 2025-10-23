#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// LCFS scheduler info
typedef struct {
    /* IMPLEMENT THIS */
    list_t *jobQueue;    // Queue holding job_t pointers
    job_t  *currentJob;    // Currently running job
} scheduler_LCFS_t;

// Creates and returns scheduler specific info
void* schedulerLCFSCreate()
{
    scheduler_LCFS_t* info = malloc(sizeof(scheduler_LCFS_t));
    if (info == NULL) {
        return NULL;
    }
    /* IMPLEMENT THIS */
    // Initialize job stack and clear current job
    info->jobQueue      = list_create(NULL);
    info->currentJob = NULL;
    if (info->jobQueue == NULL) {
        free(info);
        return NULL;
    }

    return info;
}

// Destroys scheduler specific info
void schedulerLCFSDestroy(void* schedulerInfo)
{
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    if (info == NULL) {
        return;
    }

    // destroy the job stack
    if (info->jobQueue != NULL) {
        list_destroy(info->jobQueue);
    }
    free(info);
}

// Called to schedule a new job in the queue
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// job - new job being added to the queue
// currentTime - the current simulated time
void schedulerLCFSScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime)
{
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    // push new job onto top of stack
    list_insert(info->jobQueue, job);

    // if no job is currently running schedule this one immediately
    if (list_count(info->jobQueue) == 1) {
        info->currentJob = job;
        uint64_t finishTime = currentTime + jobGetJobTime(job);
        schedulerScheduleNextCompletion(scheduler, finishTime);
    }
}

// Called to complete a job in response to an earlier call to schedulerScheduleNextCompletion
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// currentTime - the current simulated time
// Returns the job that is being completed
job_t* schedulerLCFSCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime)
{
    scheduler_LCFS_t* info = (scheduler_LCFS_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    if (info == NULL || info->currentJob == NULL) {
        return NULL;
    }

    // Remove the completed job from the stack
    job_t *finished = info->currentJob;
    list_remove(info->jobQueue, list_find(info->jobQueue, finished));

    // If there are jobs left schedule the next one
    if (list_count(info->jobQueue) > 0) {
        // next job is at the top of the stack
        list_node_t *node = list_head(info->jobQueue);
        job_t *next = (job_t*)list_data(node);
        info->currentJob = next;
        uint64_t finishTime = currentTime + jobGetJobTime(next);
        schedulerScheduleNextCompletion(scheduler, finishTime);
    } else {
        info->currentJob = NULL;
    }

    return finished;
}
