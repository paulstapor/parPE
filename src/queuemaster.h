#ifndef QUEUE_MASTER_H
#define QUEUE_MASTER_H

#include <mpi.h>
#include <pthread.h>

//#define MASTER_QUEUE_H_SHOW_COMMUNICATION

/** data to be sent to workers */
typedef struct JobData_tag {
    int jobId;
    int lenSendBuffer;
    char *sendBuffer;

    int lenRecvBuffer;
    char *recvBuffer;

    /** incremented by one, once the results have been received */
    int *jobDone;

    /** is signaled after jobDone has been incremented */
    pthread_cond_t *jobDoneChangedCondition;
    pthread_mutex_t *jobDoneChangedMutex;

    MPI_Request *recvRequest;
} JobData;

/**
 * @brief initMasterQueue Intialize queue. There can only be one queue.
 * Repeated calls won't do anything. MPI_Init has to be called before.
 */

void initMasterQueue();

/**
 * @brief queueSimulation Append work to queue.
 * @param data
 */

void queueSimulation(JobData *data);

/**
 * @brief queueSimulation Cancel the queue thread and clean up. Do not wait for finish.
 */

void terminateMasterQueue();

#endif
