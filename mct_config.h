#ifndef _MCT_CONFIG_H_
#define _MCT_CONFIG_H_

#define ARCH x86
//#define ARCH ppc

/**
 * CONFIG_64BIT - on 64bit system 
 */
#define CONFIG_64BIT

/**
 * MCC_CACHELINE_BYTES - cache line size in bytes 
 */
#define MCT_CACHELINE_BYTES 64

/** 
 * HAVE_Q_LEN - to decide whether having xxxx_queue_len() primitive
 * note this cost some performance lost due to the 
 * expensive atomic operation 
 */
#define HAVE_Q_LEN

/** 
 * CONFIG_SAME_CPU - Are the producer and consumer runing on same 
 * processor? Normally you should not define it. It's a subset of 
 * CONFIG_SMP. 
 * When you have SMP, but you'r sure producer and consumer 
 * run on same processor, CONFIG_SAME_CPU will switch off
 * all the memory barriers for performance. This is common
 * when you have producer in irq and consumer in tasklet 
 * scheduled in the same irq, or vise versa. (tasklets always
 * run on the same processor on which it's scheduled.)
 */
//#define CONFIG_SAME_CPU



#endif /*_MCC_CONFIG_H_*/
