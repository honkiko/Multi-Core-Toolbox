#ifndef _MCT_SYNC_PRIM_64_H_
#define _MCT_SYNC_PRIM_64_H_
/*
 * Synchronization primitives
 * fetch_and_store(x,v) - set x = v and return old value 
 * compare_and_store(x, cmp, v) 
 * double_cmp_and_swap(p,cmp,v)
 *
 * You can do anything you want with this file.
 *
 * Author: Hong Zhiguo <honkiko@gmail.com/honkiko@163.com>
 */

#ifdef __KERNEL__
#include "linux/types.h"  /*uint64_t*/
#else
#include <stdint.h>       /*uint64_t*/
#endif
/**
 * fetch_and_store - get old value pointed by p, and write v to *p
 * (*p) and v should be of same type, usally they are pointers
 */
#define fetch_and_store(ptr, v)                                 \
({                                                              \
    typeof(*ptr) _v = (v);                                      \
    switch (sizeof(v)) {                                        \
    case 1:                                                     \
    {                                                           \
        volatile uint8_t *_ptr = (volatile uint8_t *)(ptr);     \
        __asm__ __volatile__(                                   \
            "xchgb %0, %1\n\t"                                  \
            : "+r" (_v),                                        \
              "+m" (*_ptr)                                      \
            :                                                   \
            : "memory", "cc");                                  \
        break;                                                  \
    }                                                           \
    case 2:                                                     \
    {                                                           \
        volatile uint16_t *_ptr = (volatile uint16_t *)(ptr);   \
        __asm__ __volatile__(                                   \
            "xchgw %0, %1\n\t"                                  \
            : "+r" (_v),                                        \
              "+m" (*_ptr)                                      \
            :                                                   \
            : "memory", "cc");                                  \
        break;                                                  \
    }                                                           \
    case 4:                                                     \
    {                                                           \
        volatile uint32_t *_ptr = (volatile uint32_t *)(ptr);   \
        __asm__ __volatile__(                                   \
            "xchgl %0, %1\n\t"                                  \
            : "+r" (_v),                                        \
              "+m" (*_ptr)                                      \
            :                                                   \
            : "memory", "cc");                                  \
        break;                                                  \
    }                                                           \
    case 8:                                                     \
    {                                                           \
        volatile uint64_t *_ptr = (volatile uint64_t *)(ptr);   \
        __asm__ __volatile__(                                   \
            "xchgq %0, %1\n\t"                                  \
            : "+r" (_v),                                        \
              "+m" (*_ptr)                                      \
            :                                                   \
            : "memory", "cc");                                  \
        break;                                                  \
    }                                                           \
    default:                                                    \
        ;                                                       \
    }                                                           \
    _v;                                                         \
})

/**
 * compare_and_store - simpler symatic than compare_and_swap
 * only *p may be changed. other parameters (cmp, swap) will
 * not be changed. 
 * cmpxchgl: Compare EAX with r/m64. If equal, ZF is set 
 * and r64is loaded into r/m64. Else, clear ZF 
 * and load r/m64into EAX. 
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 * @Note: (*p) and cmp, swap must have same size
 */
#define compare_and_store(ptr, cmp, swap)                       \
({                                                              \
    unsigned char result;                                       \
    typeof(*(ptr)) _cmp = (cmp);                                \
    typeof(*(ptr)) _swap = (swap);                              \
    switch (sizeof(_cmp)) {                                     \
    case 1:                                                     \
    {                                                           \
        volatile uint8_t *_p = (volatile uint8_t *)(ptr);       \
        __asm__ __volatile__ (                                  \
            "lock ; cmpxchgb %3,%1 \n\t"                        \
            "setz %2;\n\t"                                      \
            : "+a" (_cmp),                                      \
              "+m" (*_p),                                       \
              "=r" (result)                                     \
            : "r" (_swap)                                       \
            : "cc", "memory");                                  \
        break;                                                  \
    } /*case 1*/                                                \
    case 2:                                                     \
    {                                                           \
        volatile uint16_t *_p = (volatile uint16_t *)(ptr);     \
        __asm__ __volatile__ (                                  \
            "lock ; cmpxchgw %3,%1 \n\t"                        \
            "setz %2;\n\t"                                      \
            : "+a" (_cmp),                                      \
              "+m" (*_p),                                       \
              "=r" (result)                                     \
            : "r" (_swap)                                       \
            : "cc", "memory");                                  \
        break;                                                  \
    } /*case 2*/                                                \
    case 4:                                                     \
    {                                                           \
        volatile uint32_t *_p = (volatile uint32_t *)(ptr);     \
        __asm__ __volatile__ (                                  \
            "lock ; cmpxchgl %3,%1 \n\t"                        \
            "setz %2;\n\t"                                      \
            : "+a" (_cmp),                                      \
              "+m" (*_p),                                       \
              "=r" (result)                                     \
            : "r" (_swap)                                       \
            : "cc", "memory");                                  \
        break;                                                  \
    } /*case 4*/                                                \
    case 8:                                                     \
    {                                                           \
        volatile uint64_t *_p = (volatile uint64_t *)(ptr);     \
        __asm__ __volatile__ (                                  \
            "lock ; cmpxchgq %3,%1 \n\t"                        \
            "setz %2;\n\t"                                      \
            : "+a" (_cmp),                                      \
              "+m" (*_p),                                       \
              "=r" (result)                                     \
            : "r" (_swap)                                       \
            : "cc", "memory");                                  \
        break;                                                  \
    } /*case 1*/                                                \
    default:                                                    \
        ;                                                       \
    } /*switch*/                                                \
    result;                                                     \
})


/*
 * usally this struct is used as versioned/tagged pointer
 * to deal with ABA problem
 * Double word aligned.
 */
struct _mct_double_word_t
{
    long lo;
    long hi;
} __attribute__ (( __aligned__( 2*MCT_WORDSIZE ) ));

#if (MCT_WORDSIZE==4)

/**
 * double_cmp_and_swap -  Double-Compare-And-Swap C interface on 
 * IA32 processors.
 * It's useful for many lock-free/wait-free Non-blocking data
 * structures to deal with ABA problem
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 *
 * cmpxchg8b:
 * Compare EDX:EAX with m64. 
 * If equal, set ZF and load ECX:EBX into m64. 
 * Else, clear ZF and load m64 into EDX:EAX.
 */
static inline unsigned char 
double_cmp_and_swap( volatile void *dst, void *cmp, void *swap )
{
    unsigned char result;
    volatile struct _mct_double_word_t * _dst = 
        (struct _mct_double_word_t *)dst;
    volatile struct _mct_double_word_t * _cmp = 
        (struct _mct_double_word_t *)cmp;
    volatile struct _mct_double_word_t * _swap = 
        (struct _mct_double_word_t *)swap;
    __asm__ __volatile__
    (
        "lock ; cmpxchg8b %0;\n\t"
        "setz %1;\n\t"
        : /*output*/
          "+m" ( *_dst ),
          "=r" ( result ), 
          "+d" ( _cmp->hi ),
          "+a" ( _cmp->lo )
        : /*input*/
          "c" ( _swap->hi ),
          "b" ( _swap->lo )
        : /*clobbered*/
          "cc", "memory"
    );
    return result;
}

#else /*x86-64*/
/* cmpxchg16b:
 * Compare RDX:RAX with m128. 
 * If equal, set ZF and load RCX:RBX into m128. 
 * Else, clear ZF and load m128 into RDX:RAX.
 *
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 */
static inline unsigned char 
double_cmp_and_swap( volatile void *dst, void *cmp, void *swap )
{
    unsigned char result;
    volatile struct _mct_double_word_t * _dst = 
        (struct _mct_double_word_t *)dst;
    volatile struct _mct_double_word_t * _cmp = 
        (struct _mct_double_word_t *)cmp;
    volatile struct _mct_double_word_t * _swap = 
        (struct _mct_double_word_t *)swap;
    __asm__ __volatile__
    (
        "lock ; cmpxchg16b %0;\n\t"
        "setz %1;\n\t"
        : /*output*/
          "+m" ( *_dst ),
          "=r" ( result ), 
          "+d" ( _cmp->hi ),
          "+a" ( _cmp->lo )
        : /*input*/
          "c" ( _swap->hi ),
          "b" ( _swap->lo )
        : /*clobbered*/
          "cc", "memory"
    );
    return result;
}
#endif /*MCT_WORDSIZE*/


#ifdef __KERNEL__
#include "asm/processor.h"  /*cpu_relax()*/
#else
/* nop == pause */
static inline void
cpu_relax(void)
{
    __asm__ __volatile__("rep;nop": : :"memory");
}
#endif 

#endif  /*_MCT_SYNC_PRIM_64_H_*/
