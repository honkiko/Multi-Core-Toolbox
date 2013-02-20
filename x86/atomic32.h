#ifndef _MCT_ATOMIC32_H_
#define _MCT_ATOMIC32_H_

#define LOCK_PREFIX "\n\tlock; "




typedef (void *) ptr_t;

/**
 * fetch_and_store - get old value pointed by p, and write v to *p
 */
static inline ptr_t fetch_and_store(ptr_t *p, ptr_t v)
{
    ptr_t old = v;
    __asm__ __volatile__( 
        "xchgl %0, %1\n\t"     
        : "+r" (old), 
          "+m" (*(ptr))
        : 
        : "memory", "cc");
    return old;    
} 


/**
 * compare_and_store - simpler symatic than compare_and_swap
 * only *p may be changed. other parameters (cmp, swap) will
 * not be changed. 
 * Compare EAX with r/m32. If equal, ZF is set 
 * and r32is loaded into r/m32. Else, clear ZF 
 * and load r/m32into EAX. 
 * @return: 1 if exchanged/swapped, 0 if not exchanged/swapped
 */
static inline char compare_and_store(ptr_t *p, ptr_t cmp, ptr_t swap)
{
    unsigned char result;;
    ptr_t _cmp = cmp;
    ptr_t _swap = swap;
    volatile ptr_t *_p = (volatile ptr_t *)(ptr);
    __asm__ __volatile__ (
        "lock ; cmpxchgl %3,%1 \n\t"
        "setz %2;\n\t"
        : "+a" (_cmp), 
          "+m" (*_p),
          "=r" (result)
        : "r" (_swap) 
        : "cc", "memory");    
}

typedef struct {
    int counter;
} atomic32_t;


/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

#define ATOMIC_INIT(i)  { (i) }

/**
 * atomic32_read - read atomic variable
 * @v: pointer of type atomic32_t
 *
 * Atomically reads the value of @v.
 */
static inline int atomic32_read(const atomic32_t *v)
{
    return (*(volatile int *)&(v)->counter);
}

/**
 * atomic32_set - set atomic variable
 * @v: pointer of type atomic32_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static inline void atomic32_set(atomic32_t *v, int i)
{
    v->counter = i;
}


/**
 * atomic32_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic32_t
 *
 * Atomically adds @i to @v.
 */
static inline void atomic32_add(int i, atomic32_t *v)
{
    asm volatile(LOCK_PREFIX "addl %1,%0"
             : "+m" (v->counter)
             : "ir" (i));
}

/**
 * atomic32_sub - subtract integer from atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic32_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic32_sub(int i, atomic32_t *v)
{
    asm volatile(LOCK_PREFIX "subl %1,%0"
             : "+m" (v->counter)
             : "ir" (i));
}

/**
 * atomic32_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type atomic32_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic32_sub_and_test(int i, atomic32_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "subl %2,%0; sete %1"
             : "+m" (v->counter), "=qm" (c)
             : "ir" (i) : "memory");
    return c;
}

/**
 * atomic32_inc - increment atomic variable
 * @v: pointer of type atomic32_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic32_inc(atomic32_t *v)
{
    asm volatile(LOCK_PREFIX "incl %0"
             : "+m" (v->counter));
}

/**
 * atomic32_dec - decrement atomic variable
 * @v: pointer of type atomic32_t
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic32_dec(atomic32_t *v)
{
    asm volatile(LOCK_PREFIX "decl %0"
             : "+m" (v->counter));
}

/**
 * atomic32_dec_and_test - decrement and test
 * @v: pointer of type atomic32_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic32_dec_and_test(atomic32_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "decl %0; sete %1"
             : "+m" (v->counter), "=qm" (c)
             : : "memory");
    return c != 0;
}

/**
 * atomic32_inc_and_test - increment and test
 * @v: pointer of type atomic32_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic32_inc_and_test(atomic32_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "incl %0; sete %1"
             : "+m" (v->counter), "=qm" (c)
             : : "memory");
    return c != 0;
}

/**
 * atomic32_add_negative - add and test if negative
 * @i: integer value to add
 * @v: pointer of type atomic32_t
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static inline int atomic32_add_negative(int i, atomic32_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "addl %2,%0; sets %1"
             : "+m" (v->counter), "=qm" (c)
             : "ir" (i) : "memory");
    return c;
}

#if 0
/**
 * atomic32_add_return - add integer and return
 * @i: integer value to add
 * @v: pointer of type atomic32_t
 *
 * Atomically adds @i to @v and returns @i + @v
 * @note: not valid for 386
 */
static inline int atomic32_add_return(int i, atomic32_t *v)
{
    /* Modern 486+ processor */
    return i + xadd(&v->counter, i);
}

/**
 * atomic32_sub_return - subtract integer and return
 * @v: pointer of type atomic32_t
 * @i: integer value to subtract
 *
 * Atomically subtracts @i from @v and returns @v - @i
 */
static inline int atomic32_sub_return(int i, atomic32_t *v)
{
    return atomic32_add_return(-i, v);
}
#endif

#endif
