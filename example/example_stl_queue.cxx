/*
 * This example shows normal list based queues (STL queue<,list>)
 * will run into wrong state without lock when producer and consumer
 * works parallely.  
 * After some seconds, the q.empty() will always return true while 
 * producer is doing enqueue.
 * It's not as bad as I expect (crash). But it don't work without lock.
 *
 * While spsc_queue works well in this situation. Which means more 
 * parallelism.
 *
 * Auther: Hong Zhiguo <honkiko@gmail.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pthread.h"
#include <unistd.h>
#include <sched.h>
/* the only header you need */
#include <queue>
#include <list>

std::queue<int, std::list<int>  > q;

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
    int i = 0;
    
    stick_this_thread_to_core(0);

    while (1) {
        q.push(i);
        printf("%s enqueue %d\n", producer_prefix, i);
        i++;
        usleep(producer_interval);
    }

    return NULL;
}
void * consumer_thread_func(void *arg)
{
    int x;

    stick_this_thread_to_core(1);

    while (1) {
        if (q.empty()) 
            printf("%s dequeue null\n", consumer_prefix);
        else {
            x = q.front();
            q.pop();
            printf("%s dequeue %d\n", consumer_prefix, x);
        }
   
        usleep(consumer_interval);
    }
}

int main() 
{
    pthread_t producer_thread;
    pthread_t consumer_thread;
    int ret;

    ret = pthread_create(&producer_thread, 
                    NULL, 
                    producer_thread_func, 
                    &q);
    if (ret) {
        perror("pthread_create failed");
        return -1;
    }

    ret = pthread_create(&consumer_thread, 
                    NULL, 
                    consumer_thread_func, 
                    &q);
    if (ret) {
        perror("pthread_create failed");
        return -1;
    }

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}
