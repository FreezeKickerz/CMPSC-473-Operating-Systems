#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// SJF scheduler info
typedef struct {
    /* IMPLEMENT THIS */
    list_t *jobQueue;    // Queue holding job_t pointers
    job_t  *currentJob;    // Currently running job
} scheduler_SJF_t;

// Creates and returns scheduler specific info
void* schedulerSJFCreate()
{
    scheduler_SJF_t* info = malloc(sizeof(scheduler_SJF_t));
    if (info == NULL) {
        return NULL;
    }
    /* IMPLEMENT THIS */
    info->jobQueue = list_create(NULL);
    if (!info->jobQueue) {
        free(info);
        return NULL;
    }
    info->currentJob = NULL;
    return info;
}

// Destroys scheduler specific info
void schedulerSJFDestroy(void* schedulerInfo)
{
    scheduler_SJF_t* info = (scheduler_SJF_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    list_destroy(info->jobQueue);
    free(info);
}

// Called to schedule a new job in the queue
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// job - new job being added to the queue
// currentTime - the current simulated time
void schedulerSJFScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime)
{
    scheduler_SJF_t* info = (scheduler_SJF_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    list_insert(info->jobQueue, job);
    // if no job running, schedule this one
    if (info->currentJob == NULL) {
        info->currentJob = job;
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetJobTime(job));
    }
}

// Called to complete a job in response to an earlier call to schedulerScheduleNextCompletion
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// currentTime - the current simulated time
// Returns the job that is being completed
job_t* schedulerSJFCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime)
{
    scheduler_SJF_t* info = (scheduler_SJF_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    job_t* finished = info->currentJob;
    // remove the completed job from list
    list_node_t* node = list_find(info->jobQueue, finished);
    list_remove(info->jobQueue, node);

    // scan for the shortest remaining job
    uint64_t bestTime = UINT64_MAX;
    uint64_t bestId = UINT64_MAX;
    job_t* nextJob = NULL;
    for (list_node_t* cur = list_head(info->jobQueue); cur; cur = list_next(cur)) {
        job_t* candidate = (job_t*)list_data(cur);
        uint64_t t = jobGetJobTime(candidate);
        uint64_t id = jobGetId(candidate);
        if (t < bestTime || (t == bestTime && id < bestId)) {
            bestTime = t;
            bestId = id;
            nextJob = candidate;
        }
    }

    if (nextJob) {
        info->currentJob = nextJob;
        schedulerScheduleNextCompletion(scheduler, currentTime + jobGetJobTime(nextJob));
    } else {
        info->currentJob = NULL;
    }

    return finished;
}
