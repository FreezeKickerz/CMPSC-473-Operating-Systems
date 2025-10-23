#include "channel.h"

#define unbuf_op_send            0
#define unbuf_op_receive         1
#define unbuf_op_none           -1

// Creates a new channel with the provided size and returns it to the caller
channel_t* channel_create(size_t size)
{
    channel_t* channel = malloc(sizeof(channel_t));
    if (!channel) return NULL;

    /* mutexes */
    pthread_mutex_init(&channel->chan_mutex,         NULL);
    pthread_mutex_init(&channel->select_list_mutex,  NULL);

    /* buffered-operation condition variables */
    pthread_cond_init(&channel->cond_not_full,   NULL);
    pthread_cond_init(&channel->cond_not_empty,  NULL);

    /* unbuffered rendezvous condition variables */
    pthread_cond_init(&channel->rendezvous_wait_cv,     NULL);
    pthread_cond_init(&channel->rendezvous_complete_cv, NULL);

    /* state */
    channel->buffer            = NULL;       
    channel->closed_flag       = false;
    channel->send_selectors    = list_create();
    channel->recv_selectors    = list_create();
    channel->active_unbuf_op   = unbuf_op_none ;
    channel->unbuf_stage       = 0;
    channel->is_unbuffered     = (size == 0);
    if (!channel->is_unbuffered) {
        channel->buffer = buffer_create(size);
    }
    channel->unbuf_data        = NULL;
    channel->waiting_senders   = 0;
    channel->waiting_receivers = 0;

    return channel;
}

void notify_select_senders(channel_t* channel)
{
    pthread_mutex_lock(&channel->select_list_mutex);
    for (list_node_t* node = list_head(channel->send_selectors);
         node; node = node->next)
    {
        sem_post((sem_t*)node->data);
    }
    pthread_mutex_unlock(&channel->select_list_mutex);
}

void notify_select_receivers(channel_t* channel)
{
    pthread_mutex_lock(&channel->select_list_mutex);
    for (list_node_t* node = list_head(channel->recv_selectors);
         node; node = node->next)
    {
        sem_post((sem_t*)node->data);
    }
    pthread_mutex_unlock(&channel->select_list_mutex);
}

/* add/remove selector helpers */
void add_select_sender(channel_t* channel, sem_t* sem)
{
    pthread_mutex_lock(&channel->select_list_mutex);
    list_insert(channel->send_selectors, sem);
    pthread_mutex_unlock(&channel->select_list_mutex);
}

void remove_select_sender(channel_t* channel, sem_t* sem)
{
    pthread_mutex_lock(&channel->select_list_mutex);
    list_remove(channel->send_selectors,
                list_find(channel->send_selectors, sem));
    pthread_mutex_unlock(&channel->select_list_mutex);
}

void add_select_receiver(channel_t* channel, sem_t* sem)
{
    pthread_mutex_lock(&channel->select_list_mutex);
    list_insert(channel->recv_selectors, sem);
    pthread_mutex_unlock(&channel->select_list_mutex);
}

void remove_select_receiver(channel_t* channel, sem_t* sem)
{
    pthread_mutex_lock(&channel->select_list_mutex);
    list_remove(channel->recv_selectors,
                list_find(channel->recv_selectors, sem));
    pthread_mutex_unlock(&channel->select_list_mutex);
}

static enum channel_status unbuffered_sync(channel_t* channel, int op, void** data_ptr)
{
stage0:
    if (channel->unbuf_stage == 0) {
        channel->unbuf_stage     = 1;
        channel->active_unbuf_op = op;
        channel->unbuf_data      = data_ptr;

        if (op == unbuf_op_send) {
            notify_select_receivers(channel);
            pthread_cond_broadcast(&channel->cond_not_empty);
        } else {
            notify_select_senders(channel);
            pthread_cond_broadcast(&channel->cond_not_full);
        }

        pthread_cond_wait(&channel->rendezvous_complete_cv,
                          &channel->chan_mutex);

        channel->unbuf_stage = 0;
        pthread_cond_broadcast(&channel->rendezvous_wait_cv);
        pthread_mutex_unlock(&channel->chan_mutex);
        return SUCCESS;
    }
    else if (channel->unbuf_stage == 1) {
        if (channel->active_unbuf_op == op) {
            if (op == unbuf_op_send)  channel->waiting_senders++;
            else                        channel->waiting_receivers++;

            pthread_cond_wait(&channel->rendezvous_wait_cv,
                              &channel->chan_mutex);

            if (op == unbuf_op_send)  channel->waiting_senders--;
            else                        channel->waiting_receivers--;

            goto stage0;
        }
        /* complementary operation */
        if (op == unbuf_op_receive) {
            *data_ptr = *channel->unbuf_data;
        } else { /* SEND */
            *channel->unbuf_data = *data_ptr;
        }

        channel->unbuf_stage = 2;
        pthread_cond_signal(&channel->rendezvous_complete_cv);

        pthread_mutex_unlock(&channel->chan_mutex);
        return SUCCESS;
    }
    else { /* stage 2 */
        if (op == unbuf_op_send)  channel->waiting_senders++;
        else                        channel->waiting_receivers++;

        pthread_cond_wait(&channel->rendezvous_wait_cv,
                          &channel->chan_mutex);

        if (op == unbuf_op_send)  channel->waiting_senders--;
        else                        channel->waiting_receivers--;

        goto stage0;
    }

    return GENERIC_ERROR;
}

// Writes data to the given channel
// This is a blocking call i.e., the function only returns on a successful completion of send
// In case the channel is full, the function waits till the channel has space to write the new data
// Returns SUCCESS for successfully writing data to the channel,
// CLOSED_ERROR if the channel is closed, and
// GENERIC_ERROR on encountering any other generic error of any sort
enum channel_status channel_send(channel_t* channel, void* data)
{
    if (pthread_mutex_lock(&channel->chan_mutex) != 0) return GENERIC_ERROR;

    if (channel->closed_flag) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CLOSED_ERROR;
    }

    if (channel->is_unbuffered) {
        return unbuffered_sync(channel, unbuf_op_send, &data);
    }

    /* buffered */
    while (buffer_add(channel->buffer, data) == BUFFER_ERROR) {
        if (channel->closed_flag) {
            pthread_mutex_unlock(&channel->chan_mutex);
            return CLOSED_ERROR;
        }
        pthread_cond_wait(&channel->cond_not_full, &channel->chan_mutex);
    }

    pthread_mutex_unlock(&channel->chan_mutex);
    notify_select_receivers(channel);
    pthread_cond_signal(&channel->cond_not_empty);
    return SUCCESS;
}

// Reads data from the given channel and stores it in the function's input parameter, data (Note that it is a double pointer)
// This is a blocking call i.e., the function only returns on a successful completion of receive
// In case the channel is empty, the function waits till the channel has some data to read
// Returns SUCCESS for successful retrieval of data,
// CLOSED_ERROR if the channel is closed, and
// GENERIC_ERROR on encountering any other generic error of any sort
enum channel_status channel_receive(channel_t* channel, void** data)
{
    if (pthread_mutex_lock(&channel->chan_mutex) != 0) return GENERIC_ERROR;

    if (channel->closed_flag) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CLOSED_ERROR;
    }

    if (channel->is_unbuffered) {
        return unbuffered_sync(channel, unbuf_op_receive, data);
    }

    /* buffered */
    while (buffer_remove(channel->buffer, data) == BUFFER_ERROR) {
        if (channel->closed_flag) {
            pthread_mutex_unlock(&channel->chan_mutex);
            return CLOSED_ERROR;
        }
        pthread_cond_wait(&channel->cond_not_empty, &channel->chan_mutex);
    }

    pthread_mutex_unlock(&channel->chan_mutex);
    notify_select_senders(channel);
    pthread_cond_signal(&channel->cond_not_full);
    return SUCCESS;
}

// Writes data to the given channel
// This is a non-blocking call i.e., the function simply returns if the channel is full
// Returns SUCCESS for successfully writing data to the channel,
// CHANNEL_FULL if the channel is full and the data was not added to the buffer,
// CLOSED_ERROR if the channel is closed, and
// GENERIC_ERROR on encountering any other generic error of any sort
enum channel_status channel_non_blocking_send(channel_t* channel, void* data)
{
    if (pthread_mutex_lock(&channel->chan_mutex) != 0) return GENERIC_ERROR;

    if (channel->closed_flag) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CLOSED_ERROR;
    }

    if (channel->is_unbuffered) {
        while (channel->waiting_receivers > 0 &&
               channel->unbuf_stage == 0 &&
               !list_count(channel->recv_selectors))
        {
            pthread_cond_wait(&channel->cond_not_empty, &channel->chan_mutex);
        }

        if ((channel->unbuf_stage == 1 &&
             channel->active_unbuf_op == unbuf_op_receive) ||
            list_count(channel->recv_selectors))
        {
            return unbuffered_sync(channel, unbuf_op_send, &data);
        }

        pthread_mutex_unlock(&channel->chan_mutex);
        return CHANNEL_FULL;
    }

    /* buffered */
    if (buffer_add(channel->buffer, data) == BUFFER_ERROR) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CHANNEL_FULL;
    }

    pthread_mutex_unlock(&channel->chan_mutex);
    notify_select_receivers(channel);
    pthread_cond_signal(&channel->cond_not_empty);
    return SUCCESS;
}

// Reads data from the given channel and stores it in the function's input parameter data (Note that it is a double pointer)
// This is a non-blocking call i.e., the function simply returns if the channel is empty
// Returns SUCCESS for successful retrieval of data,
// CHANNEL_EMPTY if the channel is empty and nothing was stored in data,
// CLOSED_ERROR if the channel is closed, and
// GENERIC_ERROR on encountering any other generic error of any sort
enum channel_status channel_non_blocking_receive(channel_t* channel, void** data)
{
    if (pthread_mutex_lock(&channel->chan_mutex) != 0) return GENERIC_ERROR;

    if (channel->closed_flag) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CLOSED_ERROR;
    }

    if (channel->is_unbuffered) {
        while (channel->waiting_senders > 0 &&
               channel->unbuf_stage == 0 &&
               !list_count(channel->send_selectors))
        {
            pthread_cond_wait(&channel->cond_not_full, &channel->chan_mutex);
        }

        if ((channel->unbuf_stage == 1 &&
             channel->active_unbuf_op == unbuf_op_send) ||
            list_count(channel->send_selectors))
        {
            return unbuffered_sync(channel, unbuf_op_receive, data);
        }

        pthread_mutex_unlock(&channel->chan_mutex);
        return CHANNEL_EMPTY;
    }

    /* buffered */
    if (buffer_remove(channel->buffer, data) == BUFFER_ERROR) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CHANNEL_EMPTY;
    }

    pthread_mutex_unlock(&channel->chan_mutex);
    notify_select_senders(channel);
    pthread_cond_signal(&channel->cond_not_full);
    return SUCCESS;
}

// Closes the channel and informs all the blocking send/receive/select calls to return with CLOSED_ERROR
// Once the channel is closed, send/receive/select operations will cease to function and just return CLOSED_ERROR
// Returns SUCCESS if close is successful,
// CLOSED_ERROR if the channel is already closed, and
// GENERIC_ERROR in any other error case
enum channel_status channel_close(channel_t* channel)
{
    if (pthread_mutex_lock(&channel->chan_mutex) != 0) return GENERIC_ERROR;

    if (channel->closed_flag) {
        pthread_mutex_unlock(&channel->chan_mutex);
        return CLOSED_ERROR;
    }

    channel->closed_flag = true;
    pthread_mutex_unlock(&channel->chan_mutex);

    /* wake everything up */
    notify_select_receivers(channel);
    notify_select_senders(channel);
    pthread_cond_broadcast(&channel->cond_not_empty);
    pthread_cond_broadcast(&channel->cond_not_full);
    pthread_cond_broadcast(&channel->rendezvous_wait_cv);
    pthread_cond_broadcast(&channel->rendezvous_complete_cv);

    return SUCCESS;
}

// Frees all the memory allocated to the channel
// The caller is responsible for calling channel_close and waiting for all threads to finish their tasks before calling channel_destroy
// Returns SUCCESS if destroy is successful,
// DESTROY_ERROR if channel_destroy is called on an open channel, and
// GENERIC_ERROR in any other error case
enum channel_status channel_destroy(channel_t* channel)
{
    if (!channel->closed_flag) return DESTROY_ERROR;

    /* condition variables */
    pthread_cond_destroy(&channel->cond_not_empty);
    pthread_cond_destroy(&channel->cond_not_full);
    pthread_cond_destroy(&channel->rendezvous_wait_cv);
    pthread_cond_destroy(&channel->rendezvous_complete_cv);

    /* mutexes */
    pthread_mutex_destroy(&channel->chan_mutex);
    pthread_mutex_destroy(&channel->select_list_mutex);

    /* buffer & selector lists */
    if (!channel->is_unbuffered) buffer_free(channel->buffer);
    list_destroy(channel->send_selectors);
    list_destroy(channel->recv_selectors);

    free(channel);
    return SUCCESS;
}

static void init_select(select_t* entries, size_t count, sem_t* sem)
{
    for (size_t i = 0; i < count; ++i) {
        if (entries[i].dir == SEND) add_select_sender(entries[i].channel, sem);
        else                        add_select_receiver(entries[i].channel, sem);
    }
}

static void cleanup_select(select_t* entries, size_t count, sem_t* sem)
{
    for (size_t i = 0; i < count; ++i) {
        if (entries[i].dir == SEND)
            remove_select_sender(entries[i].channel, sem);
        else
            remove_select_receiver(entries[i].channel, sem);
    }
}

// Takes an array of channels (channel_list) of type select_t and the array length (channel_count) as inputs
// This API iterates over the provided list and finds the set of possible channels which can be used to invoke the required operation (send or receive) specified in select_t
// If multiple options are available, it selects the first option and performs its corresponding action
// If no channel is available, the call is blocked and waits till it finds a channel which supports its required operation
// Once an operation has been successfully performed, select should set selected_index to the index of the channel that performed the operation and then return SUCCESS
// In the event that a channel is closed or encounters any error, the error should be propagated and returned through select
// Additionally, selected_index is set to the index of the channel that generated the error
enum channel_status channel_select(select_t* entries, size_t count, size_t* sel_idx)
{
    if (count == 0 || !entries || !sel_idx) return GENERIC_ERROR;

    pthread_mutex_t local_mutex;
    sem_t           local_sem;

    pthread_mutex_init(&local_mutex, NULL);
    sem_init(&local_sem, 0, 0);

    pthread_mutex_lock(&local_mutex);
    init_select(entries, count, &local_sem);

    while (1) {
        for (size_t i = 0; i < count; ++i) {
            select_t* e = &entries[i];

            if (e->dir == SEND) {
                if (e->channel->is_unbuffered) {
                    pthread_mutex_lock(&e->channel->chan_mutex);

                    if (e->channel->closed_flag) {
                        pthread_mutex_unlock(&e->channel->chan_mutex);
                        cleanup_select(entries, count, &local_sem);
                        return CLOSED_ERROR;
                    }

                    if ((e->channel->unbuf_stage == 1 &&
                         e->channel->active_unbuf_op == unbuf_op_receive) ||
                        list_count(e->channel->recv_selectors))
                    {
                        enum channel_status st =
                            unbuffered_sync(e->channel, unbuf_op_send,
                                            &e->data);
                        *sel_idx = i;
                        cleanup_select(entries, count, &local_sem);
                        pthread_mutex_unlock(&local_mutex);
                        sem_destroy(&local_sem);
                        pthread_mutex_destroy(&local_mutex);
                        return st;
                    }

                    pthread_mutex_unlock(&e->channel->chan_mutex);
                } else {
                    enum channel_status st = channel_non_blocking_send(e->channel, e->data);

                    if (st != CHANNEL_FULL) {
                        *sel_idx = i;
                        cleanup_select(entries, count, &local_sem);
                        pthread_mutex_unlock(&local_mutex);
                        sem_destroy(&local_sem);
                        pthread_mutex_destroy(&local_mutex);
                        return st;
                    }
                }
            }
            else { /* RECEIVE path */
                if (e->channel->is_unbuffered) {
                    pthread_mutex_lock(&e->channel->chan_mutex);

                    if (e->channel->closed_flag) {
                        pthread_mutex_unlock(&e->channel->chan_mutex);
                        cleanup_select(entries, count, &local_sem);
                        return CLOSED_ERROR;
                    }

                    if ((e->channel->unbuf_stage == 1 &&
                         e->channel->active_unbuf_op == unbuf_op_send) ||
                        list_count(e->channel->send_selectors))
                    {
                        enum channel_status st =
                            unbuffered_sync(e->channel, unbuf_op_receive,
                                            &e->data);
                        *sel_idx = i;
                        cleanup_select(entries, count, &local_sem);
                        pthread_mutex_unlock(&local_mutex);
                        sem_destroy(&local_sem);
                        pthread_mutex_destroy(&local_mutex);
                        return st;
                    }

                    pthread_mutex_unlock(&e->channel->chan_mutex);
                } else {
                    enum channel_status st = channel_non_blocking_receive(e->channel, &e->data);

                    if (st != CHANNEL_EMPTY) {
                        *sel_idx = i;
                        cleanup_select(entries, count, &local_sem);
                        pthread_mutex_unlock(&local_mutex);
                        sem_destroy(&local_sem);
                        pthread_mutex_destroy(&local_mutex);
                        return st;
                    }
                }
            }
        }
        sem_wait(&local_sem);
    }

    cleanup_select(entries, count, &local_sem);
    pthread_mutex_unlock(&local_mutex);
    sem_destroy(&local_sem);
    pthread_mutex_destroy(&local_mutex);
    return GENERIC_ERROR;
}