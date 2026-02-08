#ifndef WALIA_LIBC_PTHREAD_H
#define WALIA_LIBC_PTHREAD_H
typedef int pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER 0
static inline int pthread_mutex_init(pthread_mutex_t *mutex, const void *attr) { (void)mutex; (void)attr; return 0; }
static inline int pthread_mutex_lock(pthread_mutex_t *mutex) { (void)mutex; return 0; }
static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) { (void)mutex; return 0; }
static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) { (void)mutex; return 0; }
#endif
