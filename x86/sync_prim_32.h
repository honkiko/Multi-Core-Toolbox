#ifndef _MCT_SYNC_PRIM_32_H_
#define _MCT_SYNC_PRIM_32_H_
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

/**
 * the atomic primitives is usally operated on pointers
 */
typedef void * ptr_t;

/**
 * fetch_and_store - get old value pointed by p, and write v to *p
 */
static inline ptr_t fetch_and_store(volatile ptr_t *p, ptr_t v)
{
    __asm__ __volatile__( 
        "xchgl %0, %1\n\t"     
        : "+r" (v), 
          "+m" (*p)
        : 
        : "memory", "cc");
    return v;    
} 

/**
 * compare_and_store - simpler symatic than compare_and_swap
 * only *p may be changed. other parameters (cmp, swap) will
 * not be changed. 
 * cmpxchg: Compare EAX with r/m32. If equal, ZF is set 
 * and r32 is loaded into r/m32. Else, clear ZF 
 * and load r/m32 into EAX. 
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 */
static inline char compare_and_store(volatile ptr_t *p, ptr_t cmp, ptr_t swap)
{
    unsigned char result;
    //volatile ptr_t *_p = (volatile ptr_t *)(p);
    __asm__ __volatile__ (
        "lock ; cmpxchgl %3,%1 \n\t"
        "setz %2;\n\t"
        : "+a" (cmp), 
          "+m" (*p),
          "=r" (result)
        : "r" (swap)
        : "cc", "memory");    
    return result;
}


/*
 * usally this struct is used as versioned/tagged pointer
 * to deal with ABA problem
 */
struct _mct_double_word_t
{
    int lo;
    int hi;
} __attribute__ (( __aligned__( 8 ) ));

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
    volatile struct _mct_double_word_t * _dst = (struct _mct_double_word_t *)dst;
    volatile struct _mct_double_word_t * _cmp = (struct _mct_double_word_t *)cmp;
    volatile struct _mct_double_word_t * _swap = (struct _mct_double_word_t *)swap;
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
#endif  /*_MCT_SYNC_PRIM_32_H_*/
