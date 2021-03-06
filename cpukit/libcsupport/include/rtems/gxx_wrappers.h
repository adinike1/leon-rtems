/**
 * @file rtems/gxx_wrappers.h
 *
 * RTEMS threads compatibility routines for libgcc2.
 */

/*
 *  by: Rosimildo da Silva (rdasilva@connecttel.com)
 *
 *  Used ideas from:
 *    W. Eric Norum
 *    Canadian Light Source
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@cls.usask.ca
 *
 *  Eric sent some e-mail in the rtems-list as a start point for this
 *  module implementation.
 *
 *  $Id$
 */

#ifndef __GCC_WRAPPERS_h 
#define __GCC_WRAPPERS_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * These typedefs should match with the ones defined in the file
 * gcc/gthr-rtems.h in the gcc distribution.
 * FIXME: T.S, 2007/01/31: -> gcc/gthr-rtems.h still declares
 *                            void * __gthread_key_t;
 */
typedef struct __gthread_key_ {
	void *val; 	           /* this is switched with the task      */
	void (*dtor)(void*);   /* this remains in place for all tasks */
} __gthread_key, *__gthread_key_t;

typedef int   __gthread_once_t;
typedef void *__gthread_mutex_t;
typedef void *__gthread_recursive_mutex_t;

int rtems_gxx_once(__gthread_once_t *once, void (*func) (void));

int rtems_gxx_key_create (__gthread_key_t *key, void (*dtor) (void *));

int rtems_gxx_key_dtor (__gthread_key_t key, void *ptr);

int rtems_gxx_key_delete (__gthread_key_t key);

void *rtems_gxx_getspecific(__gthread_key_t key);

int rtems_gxx_setspecific(__gthread_key_t key, const void *ptr);

/*
 * MUTEX support
 */
void rtems_gxx_mutex_init (__gthread_mutex_t *mutex);

int rtems_gxx_mutex_lock (__gthread_mutex_t *mutex);

int rtems_gxx_mutex_destroy (__gthread_mutex_t *mutex);

int rtems_gxx_mutex_trylock (__gthread_mutex_t *mutex);

int rtems_gxx_mutex_unlock (__gthread_mutex_t *mutex);

void rtems_gxx_recursive_mutex_init(__gthread_recursive_mutex_t *mutex);

int rtems_gxx_recursive_mutex_lock(__gthread_recursive_mutex_t *mutex);

int rtems_gxx_recursive_mutex_trylock(__gthread_recursive_mutex_t *mutex);

int rtems_gxx_recursive_mutex_unlock(__gthread_recursive_mutex_t *mutex);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GCC_WRAPPERS_h */
