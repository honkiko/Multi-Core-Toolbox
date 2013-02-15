/*
 * This example shows how spsc_queue API is used with single producer
 * and single consumer.
 * It also proves that no lock is needed for spsc_queue when producer 
 * and consumer work concurrently/parallely.
 *
 * make, then run it, "pidstat -t -p pid" shows threads on different 
 * CPUs.
 *
 * Auther: Hong Zhiguo <honkiko@gmail.com>
 *
 */
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pthread.h"
#include <unistd.h>
#include <sched.h>
/* the only header you need */
#include "spsc_queue.h"

/* 
 * embed the spscq_node into your structure 
 * samillar as list/rb_node in Linux Kernel 
 */
struct user_struct {
    int user_data;
    struct spscq_node node;
};

char producer_prefix[] = "";
char consumer_prefix[] = "\t\t\t\t";
int producer_interval = 100;
int consumer_interval = 100;


int stick_this_thread_to_core(int core_id) {
   int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
   if (core_id >= num_cores)
      return -1;

   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(core_id, &cpuset);

   pthread_t current_thread = pthread_self();    
   return pthread_setaffinity_np(current_thread, 
           sizeof(cpu_set_t), &cpuset);
}

void * producer_thread_func(void *arg)
{
    struct spsc_queue *q = (struct spsc_queue *)arg;
    struct user_struct *ustruct;
    int i = 0;
    
    stick_this_thread_to_core(0);

    while (1) {
        ustruct = (struct user_struct*)malloc(
                sizeof(struct user_struct));
        ustruct->user_data = i;
        spsc_enqueue(q, &ustruct->node);
        printf("%s enqueue %d\n", producer_prefix, 
                ustruct->user_data);
        i++;
        usleep(producer_interval);
    }

    return NULL;
}
void * consumer_thread_func(void *arg)
{
    struct spsc_queue *q = (struct spsc_queue *)arg;
    struct user_struct *ustruct, *ustruct_to_free;
    struct spscq_node  *qnode, *qnode_to_free;

    stick_this_thread_to_core(1);

    while (1) {
        qnode = spsc_dequeue(q, &qnode_to_free);
        if (qnode) {
            ustruct = container_of(qnode, struct user_struct, node);
            printf("%s dequeue %d\n", consumer_prefix, 
                    ustruct->user_data);
        } else {
            printf("%s dequeue null\n", consumer_prefix);
        }
        if (qnode_to_free) {
            ustruct_to_free = container_of(qnode_to_free, 
                    struct user_struct, node);
            free(ustruct_to_free);
        }

        usleep(consumer_interval);
    }
}

int main() 
{
    pthread_t producer_thread;
    pthread_t consumer_thread;
    int ret;
    struct spsc_queue q;
    struct user_struct* dummy_node = 
        (struct user_struct*)malloc(sizeof(struct user_struct));
    spsc_queue_init(&q, &dummy_node->node); 

    ret = pthread_create(&producer_thread, 
                    NULL, 
                    producer_thread_func, 
                    &q);
    if (ret) {
        perror("pthread_create failed");
        spsc_queue_destroy(&q, free, struct user_struct, node);
        return -1;
    }

    ret = pthread_create(&consumer_thread, 
                    NULL, 
                    consumer_thread_func, 
                    &q);
    if (ret) {
        perror("pthread_create failed");
        spsc_queue_destroy(&q, free, struct user_struct, node);
        return -1;
    }

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    spsc_queue_destroy(&q, free, struct user_struct, node);
    return 0;
}
