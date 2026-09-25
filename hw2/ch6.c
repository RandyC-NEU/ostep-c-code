#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/time.h>

#define NUM_ITERS 1000

#define MEASURE_TIME(expression, clock_type, ptr_double_accum)\
double __accum = 0;\
for(int __i = 0; __i < NUM_ITERS; ++__i)\
{\
    struct timespec __time_before;\
    struct timespec __time_after;\
    (void)clock_gettime((clock_type), &__time_before);\
    (expression);\
    (void)clock_gettime((clock_type), &__time_after);\
    __accum += (__time_after.tv_sec - __time_before.tv_sec) * 1e9\
             + (__time_after.tv_nsec - __time_before.tv_nsec);\
}\
*ptr_double_accum = (__accum / (double)NUM_ITERS);\

int main(int argc, char** argv)
{
    /*----------------- Measure cost of syscall using clock_gettime ----------------------*/
    {
        int fd = open("scratch.txt", O_RDWR);
        double avg_time;
        if (fd < 0)
        {
            fprintf(stderr, "%s", "Failed to get fd");
        }
        MEASURE_TIME((void)read(fd, NULL, 0), CLOCK_MONOTONIC_RAW, &avg_time);
        printf("Avg Cost of read (in real time): %lf ns\n", avg_time);\

        close(fd);
    }

    /*------------------------- Measure cost of context switch -------------------------- */
    int fd1[2];
    int fd2[2];
    char msg_buf[64];
    double avg_time;
    const char* msg = "Hello child!";
    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(0, &mask);

    (void)pipe(fd1);
    (void)pipe(fd2);

    (void)sched_setaffinity(getpid(), sizeof(mask), &mask);

    int pid1 = fork();
    if (pid1 == 0)
    {
        (void)sched_setaffinity(getpid(), sizeof(mask), &mask);
        for(int i = 0; i < NUM_ITERS; ++i)
        {
            (void)write(fd1[1], msg,         strlen(msg));
            (void)read(fd2[0],  &msg_buf[0], sizeof(msg_buf));
        }
        exit(0);
    }
    else if (pid1 > 0)
    {
        MEASURE_TIME(
            {
                (void)read(fd1[0],  &msg_buf[0], sizeof(msg_buf));
                (void)write(fd2[1], msg,         strlen(msg));
            },
            CLOCK_MONOTONIC_RAW,
            &avg_time
        );
        (void)wait(NULL);
        printf("Avg Cost of context switch (in real time): %lf ns\n", avg_time);\
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(pid1);
    }
}
