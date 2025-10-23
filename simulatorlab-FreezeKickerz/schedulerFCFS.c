#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "job.h"
#include "linked_list.h"

// FCFS scheduler info
typedef struct {
    /* IMPLEMENT THIS */
    list_t *jobQueue;    // Queue holding job_t pointers
} scheduler_FCFS_t;

// Creates and returns scheduler specific info
void* schedulerFCFSCreate()
{
    scheduler_FCFS_t* info = malloc(sizeof(scheduler_FCFS_t));
    if (info == NULL) {
        return NULL;
    }
    /* IMPLEMENT THIS */
    info->jobQueue = list_create(NULL);
    if(info->jobQueue == NULL){
        free(info);
        return NULL;
    }
    return info;
}

// Destroys scheduler specific info
void schedulerFCFSDestroy(void* schedulerInfo)
{
    scheduler_FCFS_t* info = (scheduler_FCFS_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    if (schedulerInfo == NULL) return;
    if (info->jobQueue != NULL){
        list_destroy(info->jobQueue);
    }
    free(info);
}

// Called to schedule a new job in the queue
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// job - new job being added to the queue
// currentTime - the current simulated time
void schedulerFCFSScheduleJob(void* schedulerInfo, scheduler_t* scheduler, job_t* job, uint64_t currentTime)
{
    scheduler_FCFS_t* info = (scheduler_FCFS_t*)schedulerInfo;
    /* IMPLEMENT THIS */
    // add job to the end of the queue
    list_insert(info->jobQueue, job);

    // if no other job is running schedule this one
    if (list_count(info->jobQueue) == 1) {
        uint64_t finishTime = currentTime + jobGetJobTime(job);
        schedulerScheduleNextCompletion(scheduler, finishTime);
    }
}

// Called to complete a job in response to an earlier call to schedulerScheduleNextCompletion
// schedulerInfo - scheduler specific info from create function
// scheduler - used to call schedulerScheduleNextCompletion and schedulerCancelNextCompletion
// currentTime - the current simulated time
// Returns the job that is being completed
job_t* schedulerFCFSCompleteJob(void* schedulerInfo, scheduler_t* scheduler, uint64_t currentTime)
{
    scheduler_FCFS_t* info = (scheduler_FCFS_t*)schedulerInfo;
    /* IMPLEMENT THIS */

    // if queue is empty, nothing to complete
    if (list_count(info->jobQueue) == 0) {
        return NULL;
    }

    // remove the job at the front of the queue
    list_node_t *node = list_tail(info->jobQueue);
    job_t *finishedJob = (job_t*)list_data(node);
    list_remove(info->jobQueue, node);

    // if there is another job waiting schedule its completion
    if (list_count(info->jobQueue) > 0) {
        job_t *nextJob = (job_t*)list_data(list_tail(info->jobQueue));
        uint64_t nextFinish = currentTime + jobGetRemainingTime(nextJob);
        schedulerScheduleNextCompletion(scheduler, nextFinish);
    }

    return finishedJob;
}
