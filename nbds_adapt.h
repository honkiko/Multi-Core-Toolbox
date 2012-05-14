#ifndef _NBDS_ADAPT_H
#define _NBDS_ADAPT_H

#ifdef CONFIG_SAME_CPU
  #undef smp_mb()
  #undef smp_rmb()
  #undef smp_wmb()
  #undef smp_read_barrier_depends()
  #define smp_mb()        barrier()
  #define smp_rmb()       barrier()
  #define smp_wmb()       barrier()
  #define smp_read_barrier_depends()      do { } while (0)
#endif

  #define NBDS_CACHELINE_BYTES SMP_CACHE_BYTES

#ifndef __KERNEL__
  #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

  #define container_of(ptr, type, member) ({                      \
       const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
       (type *)( (char *)__mptr - offsetof(type,member) );})
       
  
 
#endif

#endif
