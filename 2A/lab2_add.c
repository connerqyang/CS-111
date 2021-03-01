//NAME: Conner Yang
//EMAIL: conneryang@g.ucla.edu
//ID: 905417287

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

long long sum = 0;
pthread_t* threads = NULL;
int num_iterations = 1;
int num_threads = 1;

    // Option-getters
int opt_yield = 0;
int sync_type = 0;

    // Set locks
pthread_mutex_t mutex;
long lock = 0;

void add(long long *pointer, long long value)
{
    long long sum = *pointer + value;
    if (opt_yield)
    {
        sched_yield();
    }
    *pointer = sum;
}

void c_add(long long *pointer, long long value)
{
    
    long long prev, sum;
    do
    {
        prev = *pointer;
        sum = prev + value;
        if (opt_yield)
        {
            sched_yield();
        }
    }
    while (__sync_val_compare_and_swap(pointer, prev, sum) != prev);
}

void* thread_worker(void* arg)
{
    for (int i = 0; i < num_iterations; i++)
    {
        switch (sync_type)
        {
            case 'm': // Mutex
                if (pthread_mutex_lock(&mutex) != 0)
                {
                    fprintf(stderr, "Error locking mutex lock\n");
                    exit(1);
                }
                add(&sum, 1);
                if (pthread_mutex_unlock(&mutex) != 0)
                {
                    fprintf(stderr, "Error unlocking mutex lock\n");
                    exit(1);
                }
                if (pthread_mutex_lock(&mutex) != 0)
                {
                    fprintf(stderr, "Error locking mutex lock\n");
                    exit(1);
                }
                add(&sum, -1);
                if (pthread_mutex_unlock(&mutex) != 0)
                {
                    fprintf(stderr, "Error unlocking mutex lock\n");
                    exit(1);
                }
                break;
            case 's':  // Spinlock
                while (__sync_lock_test_and_set(&lock, 1));
                add(&sum, 1);
                __sync_lock_release(&lock);
                while (__sync_lock_test_and_set(&lock, 1));
                add(&sum, -1);
                __sync_lock_release(&lock);
                break;
            case 'c':  // Compare-and-swap
                c_add(&sum, 1);
                c_add(&sum, -1);
                break;
            default:
                add(&sum, 1);
                add(&sum, -1);
                break;
        }
    }
    return arg;
}

void process_args(int argc, char * argv[])
{
        // Collect options
    int option;
    
    static struct option options[] = {
      {"iterations", required_argument, NULL, 'i'},
      {"threads", required_argument, NULL, 't'},
      {"yield", no_argument, NULL, 'y'},
      {"sync", required_argument, NULL, 's'},
      {0, 0, 0, 0}
    };
    
    while (1)
    {
        option = getopt_long(argc, argv, "i:t:ys:", options, NULL);
        if (option == -1) break;
        switch (option)
        {
          case 'i':
                num_iterations = atoi(optarg);
                if (num_iterations < 0)
                {
                    fprintf(stderr, "The number of iterations must be positive\n");
                }
                break;
          case 't':
                num_threads = atoi(optarg);
                if (num_threads < 0)
                {
                    fprintf(stderr, "The number of threads must be positive\n");
                }
                break;
          case 'y':
                opt_yield = 1;
                break;
          case 's':
                if (optarg[0] != 'm' && optarg[0] != 's' && optarg[0] != 'c')
                {
                    fprintf(stderr, "Error: Correct usage is --sync=m, s, or c\n");
                    exit(1);
                }
                sync_type = optarg[0];
                break;
          default:
                fprintf(stderr, "Correct options: --iterations= --threads=: %s\n", strerror(errno));
                exit(1);
        }
    }
}

void print_out(long long total_runtime)
{
        // Get average time per operation
    int operations = 2 * num_threads * num_iterations;
    long long avg_time_per_op = total_runtime / operations;

    char temp_out[50] = "";
    strcat(temp_out, "add");

    if (opt_yield)
    {
        strcat(temp_out, "-yield");
    }

    //get name of test
    switch(sync_type)
    {
        case 0:  //no sync option given
            strcat(temp_out, "-none");
            break;
        case 'c':
            strcat(temp_out, "-c");
            break;
        case 's':
            strcat(temp_out, "-s");
            break;
        case 'm':
            strcat(temp_out, "-m");
            break;
        default:
            break;
    }

    fprintf(stdout, "%s,%d,%d,%d,%lld,%lld,%lld\n", temp_out, num_threads, num_iterations, operations, total_runtime, avg_time_per_op, sum);
}

void free_memory(void)
{
    if (threads != NULL)
    {
        free(threads);
    }
}

int main(int argc, char* argv[])
{
    process_args(argc, argv);
    

    struct timespec start_runtime, end_runtime;
    clock_gettime(CLOCK_MONOTONIC, &start_runtime);

    threads = malloc(num_threads * sizeof(pthread_t));
    if (threads == NULL)
    {
        fprintf(stderr, "Error with malloc for threads\n");
        exit(1);
    }
    
    atexit(free_memory);        // Frees memory from malloc at exit

    if (sync_type == 'm')
    {
        if (pthread_mutex_init(&mutex, NULL) != 0)
        {
            fprintf(stderr, "Error intiializing mutex lock\n");
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, &thread_worker, NULL) != 0)
        {
            fprintf(stderr, "Could not create num_threads\n");
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            fprintf(stderr, "Could not join threads\n");
            exit(1);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_runtime);

    long total_runtime = 1000000000 * (end_runtime.tv_sec - start_runtime.tv_sec) + end_runtime.tv_nsec - start_runtime.tv_nsec;

    print_out(total_runtime);

    exit(0);
}
