#ifndef _MCT_ATOMIC32_H_
#define _MCT_ATOMIC32_H_
#include "mct_config.h"

#ifndef CONFIG_64BIT
#include "atomic32.h"
#define atomic_t atomic32_t
#define atomic_read atomic32_read
#define atomic_set atomic32_set
#define atomic_add atomic32_add
#define atomic_sub atomic32_sub
#define atomic_inc atomic32_inc
#define atomic_dec atomic32_dec
#define atomic_dec_and_test atomic32_dec_and_test
#define atomic_inc_and_test atomic32_inc_and_test
#define atomic_add_negative atomic32_add_negative
#define atomic_add_return atomic32_add_return
#define atomic_sub_return atomic32_sub_return
#else /*CONFIG_64BIT*/
#include "atomic64.h"
#define atomic_t atomic64_t
#define atomic_read atomic64_read
#define atomic_set atomic64_set
#define atomic_add atomic64_add
#define atomic_sub atomic64_sub
#define atomic_inc atomic64_inc
#define atomic_dec atomic64_dec
#define atomic_dec_and_test atomic64_dec_and_test
#define atomic_inc_and_test atomic64_inc_and_test
#define atomic_add_negative atomic64_add_negative
#define atomic_add_return atomic64_add_return
#define atomic_sub_return atomic64_sub_return

#endif /*CONFIG_64BIT*/

#endif /*_MCC_ATOMIC32_H_*/
