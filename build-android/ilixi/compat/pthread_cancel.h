#ifdef __ANDROID__
#ifndef HAVE_PTHREAD_CANCEL
#include <pthread.h>
enum
{
  PTHREAD_CANCEL_ENABLE,
  PTHREAD_CANCEL_DISABLE,
  PTHREAD_CANCEL_DEFERRED,
  PTHREAD_CANCELED
};

extern void pthread_cancel_initialize (void);
extern void pthread_cancel_deinitialize (void);
extern int pthread_create_cancel( pthread_t *thread,
                           const pthread_attr_t *attr,
                           void *( *routine ) ( void* ),
                           void *arg );
extern int pthread_setcancelstate( int state, int *oldstate );
extern int pthread_setcanceltype( int state, int *oldstate );
extern int pthread_cancel( pthread_t thread_id );
extern void pthread_testcancel( void );
extern int pthread_cond_wait_cancel( pthread_cond_t *cond,
                                     pthread_mutex_t *mutex );
extern int pthread_cond_timedwait_cancel( pthread_cond_t *cond,
                                          pthread_mutex_t *mutex,
                                          const struct timespec *abstime );
                                          
                                          
#define unlikely
#endif // PTHREAD_CANCEL
#endif // __ANDROID__
