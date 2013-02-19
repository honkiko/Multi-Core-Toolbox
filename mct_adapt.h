#ifndef _MCT_ADAPT_H_
#define _MCT_ADAPT_H_
#include "mct_config.h"
#ifdef __KERNEL__
    #include <asm/barrier.h>        /*smp_mb*/
    #include <linux/kernel.h>       /*container_of*/

    /* The default memory management routine for kernel*/
    #ifndef MCT_MALLOC
        #define MCT_MALLOC(n) kmalloc((n), GFP_KERNEL) 
    #endif
    #ifndef MCT_FREE
        #define MCT_FREE kfree
    #endif

    /*derive MCT_WORDSIZE from CONFIG_XXBIT*/
    #ifndef MCT_WORDSIZE
        #ifdef CONFIG_64BIT
            #define MCT_WORDSIZE 8
        #endif /*CONFIG_64BIT*/
        #ifdef CONFIG_32BIT
            #define MCT_WORDSIZE 4
        #endif /*CONFIG_32BIT*/
    #endif /*MCT_WORDSIZE*/
    #ifndef MCT_WORDSIZE
        #error "neither CONFIG_64BIT nor CONFIG_32BIT defined"
    #endif /*CONFIG_32BIT*/

    /*derive MCT_CACHELINE_BYTES from SMP_CACHE_BYTES*/
    #ifndef MCT_CACHELINE_BYTES
        #include <linux/cache.h>        /*SMP_CACHE_BYTES*/
        #ifdef SMP_CACHE_BYTES
            #define MCT_CACHELINE_BYTES SMP_CACHE_BYTES
        #endif 
    #endif /*MCT_CACHELINE_BYTES*/

#else /*ifdef __KERNEL__*/
    #include <stddef.h>             /*offsetof*/

    /*atomic_t and barriers */
    #if (ARCH==x86)
        #include "x86/barrier.h"
        #include "x86/atomic.h" 
    #endif

    /* The default memory management routine for user space*/
    #include <stdlib.h>             /*malloc*/
    #ifndef MCT_MALLOC
        #define MCT_MALLOC(n) malloc(n)
    #endif
    #ifndef MCT_FREE
        #define MCT_FREE free
    #endif

    /*derive MCT_WORDSIZE from __SIZEOF_POINTER__*/
    #ifndef MCT_WORDSIZE
        #define MCT_WORDSIZE (__SIZEOF_POINTER__)
    #endif /*MCT_WORDSIZE*/

    /*default MCT_CACHELINE_BYTES*/
    #ifndef MCT_CACHELINE_BYTES
        #define MCT_CACHELINE_BYTES 64
    #endif

    #define container_of(ptr, type, member) ({                      \
           const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
           (type *)( (char *)__mptr - offsetof(type,member) );})
   /*
   * Prevent the compiler from merging or refetching accesses.  The compiler
   * is also forbidden from reordering successive instances of ACCESS_ONCE(),
   * but only when the compiler is aware of some particular ordering.  One way
   * to make the compiler aware of ordering is to put the two invocations of
   * ACCESS_ONCE() in different C statements.
   *
   * This macro does absolutely -nothing- to prevent the CPU from reordering,
   * merging, or refetching absolutely anything at any time.  Its main intended
   * use is to mediate communication between process-level code and irq/NMI
   * handlers, all running on the same CPU.
   */
   #define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))
   
#endif /*__KERNEL__*/


#if (ARCH==x86)                 /*primitives of x86*/
    #if (MCT_WORDSIZE==4)
    #include "x86/dcas_on_32.h"
    #endif
    #if (MCT_WORDSIZE==8)
    #include "x86/dcas_on_64.h"
    #endif
#endif


#endif
