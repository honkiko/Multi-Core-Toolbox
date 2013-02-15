#ifndef _MCT_DCAS_H_
#define _MCT_DCAS_H_
#include <stdint.h>


struct _uint128_t
{
    uint64_t lo;
    uint64_t hi;
} __attribute__ (( __aligned__( 16 ) ));

typedef struct _uint128_t uint128_t;

/* cmpxchg16b:
 * Compare RDX:RAX with m128. 
 * If equal, set ZF and load RCX:RBX into m128. 
 * Else, clear ZF and load m128 into RDX:RAX.
 *
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 */
static inline unsigned char 
dcas( volatile void *dst, void *cmp, void *swap )
//dw_cas( volatile uint128_t * dst, uint128_t *cmp, uint128_t *swap )
{
    unsigned char result;
    volatile uint128_t * _dst = (uint128_t *)dst;
    volatile uint128_t * _cmp = (uint128_t *)cmp;
    volatile uint128_t * _swap = (uint128_t *)swap;
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
#endif  /*_MCT_DCAS_H_*/
