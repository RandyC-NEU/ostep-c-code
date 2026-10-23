#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>

#define MEASURE_TIME(expression, clock_type, num_iters, ptr_double_accum)\
double __accum = 0;\
for(long __i = 0; __i < (num_iters); ++__i)\
{\
    struct timespec __time_before;\
    struct timespec __time_after;\
    (void)clock_gettime((clock_type), &__time_before);\
    expression;\
    (void)clock_gettime((clock_type), &__time_after);\
    __accum += (__time_after.tv_sec - __time_before.tv_sec) * 1e9\
             + (__time_after.tv_nsec - __time_before.tv_nsec);\
}\
*ptr_double_accum = (__accum / (double)(num_iters));\

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <num_pages> <num_iters>\n", argv[0]);
        exit(1);
    }

    long num_pages = strtol(argv[1], NULL, 10);
    long num_iters = strtol(argv[2], NULL, 10);
    if (num_pages <= 0 || num_iters <= 0)
    {
        fprintf(stderr, "num_pages and num_iters must be positive\n");
        exit(1);
    }

    /* Pin to one CPU so every run hits the same TLB*/
#ifdef __linux__
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    (void)sched_setaffinity(getpid(), sizeof(mask), &mask);
#endif

    long page_size = sysconf(_SC_PAGESIZE);
    long jump = page_size / sizeof(int);
    int* a = malloc(num_pages * page_size);
    if (a == NULL)
    {
        fprintf(stderr, "Failed to allocate %ld pages\n", num_pages);
        exit(1);
    }

    /* Touch every page up front so demand-zero page faults aren't timed (found on google that this might be helpful)*/
    memset(a, 0, num_pages * page_size);

    /*----------------- Measure cost of one pass over num_pages pages ----------------------*/
    double avg_time;
    MEASURE_TIME(
        {
            for (long i = 0; i < num_pages * jump; i += jump)
            {
                a[i] += 1;
            }
        },
        CLOCK_MONOTONIC,
        num_iters,
        &avg_time
    );

    printf("Page size: %ld bytes\n", page_size);
    printf("Avg Cost of pass over %ld pages (in real time): %lf ns\n", num_pages, avg_time);
    printf("Avg Cost per page access (in real time): %lf ns\n", avg_time / (double)num_pages);

    free(a);
    return 0;
}
