/*****************************************************************************
 * pthread_cancel.c: pthread deferred cancellation replacement
 *****************************************************************************
 * Copyright © 2011 VideoLAN
 *
 * Author: Jean-Philippe André <jpeg # videolan.org>
 *
 * License: LGPLv2.1+
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "pthread_cancel.h"
#include <pthread.h>
#include <assert.h>

/*
 * This file provide fixups for the following functions:
 * - pthread_cancel
 * - pthread_testcancel
 * - pthread_setcancelstate
 * As a result of the implementation design, we must also wrap
 * pthread_create into a cancellation-aware function
 */

static void* thread_wrapper_routine (void *arg);
static void  thread_cancel_handler (int signal);
static void  thread_cancel_destructor (void *data);

// Functions used by LibVLC
void pthread_cancel_initialize (void);
void pthread_cancel_deinitialize (void);

/*
 * Some static variables used for initialization
 */
static pthread_key_t cancel_key = 0; /// Key for the thread-local variable

/*
 * These objects define the thread-local variable tracking the thread's
 * cancellation status (cancellable, cancelled)
 */
typedef struct cancel_t cancel_t;
struct cancel_t
{
    int state;      /// PTHREAD_CANCEL_ENABLE (0) or PTHREAD_CANCEL_DISABLE (1)
    pthread_cond_t *cond; /// Non-null if thread waiting on cond

    /* Booleans at the end for packing purposes */
    int cancelled; /// Non-zero means this thread has been cancelled
};

/*
 * This is the thread wrapper data
 */
typedef struct thread_wrapper_t thread_wrapper_t;
struct thread_wrapper_t
{
    void     *(*routine) (void*); /// Main thread routine to call
    void     *arg;                /// Argument to pass to the thread routine
    cancel_t *cancel; /// The cancel structure is allocated before the thread
};

/**
 * Initialize pthread cancellation
 * Creates thread-local variable's key and catches SIGRTMIN in main thread
 **/
void pthread_cancel_initialize (void)
{
    // Set up signal handler
    struct sigaction act;
    memset (&act, 0, sizeof (act));
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = thread_cancel_handler;
    sigaction (SIGRTMIN, &act, NULL);

    // Create thread-local variable key
    pthread_key_create (&cancel_key, thread_cancel_destructor);
}

/**
 * Deinitialize pthread cancellation
 **/
void pthread_cancel_deinitialize (void)
{
    struct sigaction act;
    memset (&act, 0, sizeof (act));
    sigemptyset (&act.sa_mask);
    sigaction (SIGRTMIN, &act, NULL);
    pthread_key_delete (cancel_key);
    cancel_key = 0;
}

/**
 * Replacement for pthread_cancel
 * Sends a SIGRTMIN signal to the thread
 **/
int pthread_cancel (pthread_t thread_id)
{
    return pthread_kill (thread_id, SIGRTMIN);
}

/**
 * Replacement for pthread_create with support for cancellation
 **/
int pthread_create_cancel (pthread_t *thread_id,
                           const pthread_attr_t *attr,
                           void *(*routine) (void *),
                           void *arg)
{
    thread_wrapper_t *wrapper_data =
            (thread_wrapper_t*) calloc (1, sizeof (thread_wrapper_t));
    if (unlikely (!wrapper_data))
        return -1;

    wrapper_data->routine = routine;
    wrapper_data->arg     = arg;
    wrapper_data->cancel  = (cancel_t*) calloc (1, sizeof (cancel_t));

    if (unlikely (!wrapper_data->cancel))
    {
        free (wrapper_data);
        return -1;
    }

    int ret = pthread_create (thread_id, attr, thread_wrapper_routine,
                              wrapper_data);
    if (unlikely (ret != 0))
    {
        // The thread wrapper should free these variables but it didn't start
        free (wrapper_data->cancel);
        free (wrapper_data);
    }
    return ret;
}

/**
 * Thread wrapper
 * Sets up signal handlers and thread-local data before running the routine
 **/
static void* thread_wrapper_routine (void *arg)
{
    thread_wrapper_t *wrapper_data = (thread_wrapper_t*) arg;

    // Set up signal handler
    struct sigaction act;
    memset (&act, 0, sizeof (act));
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = thread_cancel_handler;
    sigaction (SIGRTMIN, &act, NULL);

    // Place specific data (cancel state stack)
    cancel_t *canc = wrapper_data->cancel;
    memset (canc, 0, sizeof (cancel_t));
    if (unlikely (pthread_setspecific (cancel_key, canc) != 0))
        return NULL;

    void *ret = wrapper_data->routine (wrapper_data->arg);
    // Don't free wrapper_data->cancel. It will be destroyed automatically.
    free (wrapper_data);
    return ret;
}

/**
 * Change thread's cancellation state (enable/disable)
 **/
int pthread_setcancelstate (int state, int *oldstate)
{
    cancel_t *canc = pthread_getspecific (cancel_key);
    if (unlikely (canc == NULL))
    {
        /// FIXME
        // Let's just add this missing variable since the main thread
        // uses these features but wasn't created by pthread_create_cancel
        canc = (cancel_t*) calloc (1, sizeof (cancel_t));
        pthread_setspecific (cancel_key, canc);
    }

    if (oldstate)
        *oldstate = canc->state;
    canc->state = state;

    if (state == PTHREAD_CANCEL_ENABLE)
        pthread_testcancel ();

    return 0;
}

int pthread_setcanceltype (int state, int *oldstate)
{
    return 0;
}

/**
 * Create a cancellation point
 **/
void pthread_testcancel (void)
{
    cancel_t *canc = pthread_getspecific (cancel_key);
    if (unlikely (!canc))
        return; // Don't mess with the main thread

    assert (canc->cond == NULL);

    if (canc->cancelled) // Don't check PTHREAD_CANCEL_ENABLE
        pthread_exit (NULL);
}

/**
 * Cancellation signal handler
 **/
static void thread_cancel_handler (int signal)
{
    assert (signal == SIGRTMIN);

    cancel_t *canc = (cancel_t*) pthread_getspecific (cancel_key);
    if (unlikely (!canc))
        return; // Main thread, can't be cancelled

    canc->cancelled = 1;
    if (canc->cond)
    {
        /* Wakeup all threads waiting on cond. As we are supposed to expect
         * spurious wakeups, this should not be an issue
         */
        pthread_cond_t *cond = canc->cond;
        canc->cond = NULL;
        pthread_cond_broadcast (cond);
        /* FIXME: not calling pthread_exit (crashes in input thread). Why? */
        // pthread_exit (NULL);
        return;
    }
    if (canc->state == PTHREAD_CANCEL_ENABLE)
        pthread_exit (NULL);
}

/**
 * Destroy a cancel_t variable. Nothing fancy.
 **/
static void thread_cancel_destructor (void *data)
{
    cancel_t *canc = (cancel_t*) data;
    free (canc);
}

int pthread_cond_wait_cancel( pthread_cond_t *cond,
                                     pthread_mutex_t *mutex )
{
    int oldstate;
    pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &oldstate);
    cancel_t *canc = pthread_getspecific (cancel_key);
    if (canc)
    {
        assert (!canc->cond);
        canc->cond = cond;
    }

    int ret = pthread_cond_wait (cond, mutex);

    if (canc)
        canc->cond = NULL;

    /// FIXME: We should call testcancel() here, but it leads to crashes.
    // pthread_testcancel ();
    pthread_setcancelstate (oldstate, NULL);
    return ret;
}

int pthread_cond_timedwait_cancel( pthread_cond_t *cond,
                                   pthread_mutex_t *mutex,
                                   const struct timespec *abstime )
{
    int oldstate;
    pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &oldstate);
    cancel_t *canc = pthread_getspecific (cancel_key);
    if (canc)
    {
        assert (!canc->cond);
        canc->cond = cond;
    }

    int ret = pthread_cond_timedwait (cond, mutex, abstime);

    if (canc)
        canc->cond = NULL;

    /// FIXME: We should call testcancel() here, but it leads to crashes.
    // pthread_testcancel ();
    pthread_setcancelstate (oldstate, NULL);
    return ret;
}
