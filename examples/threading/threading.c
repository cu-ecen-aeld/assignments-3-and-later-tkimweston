#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    usleep(thread_func_args->wait_entry * 1000);
    pthread_mutex_lock(thread_func_args->mutex);

    usleep(thread_func_args->wait_exit * 1000);
    thread_func_args->thread_complete_success = true;

    pthread_mutex_unlock(thread_func_args->mutex);

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    int rc;
    struct thread_data *td = (struct thread_data *)malloc(sizeof(struct thread_data));
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    //rc = pthread_mutex_init(mutex, NULL);
    //if (rc != 0) 
//	    return false;
    td->mutex = mutex;
    td->wait_entry = wait_to_obtain_ms;
    td->wait_exit = wait_to_release_ms;
    rc = pthread_create(thread, NULL, threadfunc, (void *)td);
    if (rc != 0)  {
	    free(td);
	    return false;
    }
    //pthread_join(*thread, NULL);
    return true;
}

