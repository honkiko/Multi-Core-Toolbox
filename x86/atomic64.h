#ifndef _MCT_ATOMIC64_H_
#define _MCT_ATOMIC64_H_

#define LOCK_PREFIX "\n\tlock; "

#ifdef CONFIG_64BIT
typedef struct {
    long counter;
} atomic64_t;

/* The 64-bit atomic type */

#define ATOMIC64_INIT(i)    { (i) }

/**
 * atomic64_read - read atomic64 variable
 * @v: pointer of type atomic64_t
 *
 * Atomically reads the value of @v.
 * Doesn't imply a read memory barrier.
 */
static inline long atomic64_read(const atomic64_t *v)
{
    return (*(volatile long *)&(v)->counter);
}

/**
 * atomic64_set - set atomic64 variable
 * @v: pointer to type atomic64_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static inline void atomic64_set(atomic64_t *v, long i)
{
    v->counter = i;
}

/**
 * atomic64_add - add integer to atomic64 variable
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v.
 */
static inline void atomic64_add(long i, atomic64_t *v)
{
    asm volatile(LOCK_PREFIX "addq %1,%0"
             : "=m" (v->counter)
             : "er" (i), "m" (v->counter));
}

/**
 * atomic64_sub - subtract the atomic64 variable
 * @i: integer value to subtract
 * @v: pointer to type atomic64_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic64_sub(long i, atomic64_t *v)
{
    asm volatile(LOCK_PREFIX "subq %1,%0"
             : "=m" (v->counter)
             : "er" (i), "m" (v->counter));
}

/**
 * atomic64_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer to type atomic64_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic64_sub_and_test(long i, atomic64_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "subq %2,%0; sete %1"
             : "=m" (v->counter), "=qm" (c)
             : "er" (i), "m" (v->counter) : "memory");
    return c;
}

/**
 * atomic64_inc - increment atomic64 variable
 * @v: pointer to type atomic64_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic64_inc(atomic64_t *v)
{
    asm volatile(LOCK_PREFIX "incq %0"
             : "=m" (v->counter)
             : "m" (v->counter));
}

/**
 * atomic64_dec - decrement atomic64 variable
 * @v: pointer to type atomic64_t
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic64_dec(atomic64_t *v)
{
    asm volatile(LOCK_PREFIX "decq %0"
             : "=m" (v->counter)
             : "m" (v->counter));
}

/**
 * atomic64_dec_and_test - decrement and test
 * @v: pointer to type atomic64_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic64_dec_and_test(atomic64_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "decq %0; sete %1"
             : "=m" (v->counter), "=qm" (c)
             : "m" (v->counter) : "memory");
    return c != 0;
}

/**
 * atomic64_inc_and_test - increment and test
 * @v: pointer to type atomic64_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic64_inc_and_test(atomic64_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "incq %0; sete %1"
             : "=m" (v->counter), "=qm" (c)
             : "m" (v->counter) : "memory");
    return c != 0;
}

/**
 * atomic64_add_negative - add and test if negative
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static inline int atomic64_add_negative(long i, atomic64_t *v)
{
    unsigned char c;

    asm volatile(LOCK_PREFIX "addq %2,%0; sets %1"
             : "=m" (v->counter), "=qm" (c)
             : "er" (i), "m" (v->counter) : "memory");
    return c;
}
#endif


#endif
