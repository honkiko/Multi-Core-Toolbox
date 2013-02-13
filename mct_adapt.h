#ifndef _MCT_ADAPT_H_
#define _MCT_ADAPT_H_
#include "mct_config.h"
#ifdef __KERNEL__
    #include <asm/barrier.h>        /*smp_mb*/
    #include <linux/cache.h>        /*SMP_CACHE_BYTES*/
    #include <linux/kernel.h>       /*container_of*/
#else
    #include <stddef.h>             /*offsetof*/
    #if (ARCH==x86)                 /*primitives of x86*/
        #include "x86/barrier.h"
        #include "x86/atomic.h" 
    #endif
    #define container_of(ptr, type, member) ({                      \
           const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
           (type *)( (char *)__mptr - offsetof(type,member) );})
#endif /*__KERNEL__*/

#ifndef MCT_CACHELINE_BYTES
    #ifdef SMP_CACHE_BYTES
        #define MCT_CACHELINE_BYTES SMP_CACHE_BYTES
    #else
        #define MCT_CACHELINE_BYTES 64
    #endif
#endif

#endif
