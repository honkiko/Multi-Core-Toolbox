#ifndef _MCT_DCAS_ON_32_H_
#define _MCT_DCAS_ON_32_H_
/*
 * dcas: Double-Compare-And-Swap C interface on IA32 processors
 * It's useful for many lock-free/wait-free Non-blocking data
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
    int lo;
    int hi;
} __attribute__ (( __aligned__( 8 ) ));

/* cmpxchg8b:
 * Compare EDX:EAX with m64. 
 * If equal, set ZF and load ECX:EBX into m64. 
 * Else, clear ZF and load m64 into EDX:EAX.
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
#endif  /*_MCT_DCAS_ON_32_H_*/
