#ifndef _MCT_DCAS_ON_64_H_
#define _MCT_DCAS_ON_64_H_
/*
 * dcas: Double-Compare-And-Swap C interface on x86-64 processors
 * It's useful for many lock-free/wait-free/Non-blocking data
 * structures to deal with ABA problem
 *
 * You can do anything you want with this file.
 *
 * Author: Hong Zhiguo <honkiko@gmail.com/honkiko@163.com>
 */



/*
 * usally this struct is used as versioned/tagged pointer
 * to deal with ABA problem
 */
struct _mct_double_word_t
{
    long lo;
    long hi;
} __attribute__ (( __aligned__( 16 ) ));


/* cmpxchg16b:
 * Compare RDX:RAX with m128. 
 * If equal, set ZF and load RCX:RBX into m128. 
 * Else, clear ZF and load m128 into RDX:RAX.
 *
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 */
static inline unsigned char 
dcas( volatile void *dst, void *cmp, void *swap )
{
    unsigned char result;
    volatile struct _mct_double_word_t * _dst = (struct _mct_double_word_t *)dst;
    volatile struct _mct_double_word_t * _cmp = (struct _mct_double_word_t *)cmp;
    volatile struct _mct_double_word_t * _swap = (struct _mct_double_word_t *)swap;
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
#endif  /*_MCT_DCAS_ON_64_H_*/
