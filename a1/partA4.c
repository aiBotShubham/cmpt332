/* CMPT 332 -- Fall 2017
* Assignment 1
* Derek Perrin      dmp450 11050915
* Dominic McKeith   dom258 11184543
*/

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#include "common.h"

#define MAX_COUNT 200

size_t square_counts = 0;
struct timespec start_time;

void error_exit(char* error_message){
    fprintf(stderr, "%s", error_message);
    exit(EXIT_FAILURE);
}

void incr_func(void){
    square_counts++;
}

void child_exit(int sig) {
    struct timespec end_time;
    if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
        error_exit("partA4.c: Could not get end time\n");
    }
    /* Measure the time difference since the process started and print it */
    int elapsed_time = (int)(end_time.tv_sec - start_time.tv_sec)*1000 +
        (int)(end_time.tv_nsec - start_time.tv_nsec)/1000000;

    int my_pid = (int) getpid();
    /* We shouldn't use printf because it's not synchronous. */
    printf("Process %d signalled to exit: No. of Square calls: %lu, Elapsed "
           "time: %d ms\n", my_pid, square_counts, elapsed_time);
    exit(1);
}

void child_func(int size){
    struct timespec end_time;

    /* Set up the sigaction struct to deal with signals */
    struct sigaction sa;
    sa.sa_handler = child_exit;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0) {
        error_exit("partA4.c: Could not get start time\n");
    }

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    for (size_t i = 1; i <= size; ++i){
        Square(i);
    }
    
    /* Measure the time difference since the process started and print it */
    if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
        error_exit("partA4.c: Could not get end time\n");
    }
    int elapsed_time = (int)(end_time.tv_sec - start_time.tv_sec)*1000 +
        (int)(end_time.tv_nsec - start_time.tv_nsec)/1000000;

    /* print out the info we're supposed to print out */
    int my_pid = (int) getpid();
    printf("Process %d finished normally. No. of Square calls: %lu, Elapsed"
           " time: %d ms\n", my_pid, square_counts, elapsed_time);
    
    exit(0);
}

int timer(int deadline, int num_procs, pid_t* pids){    
    if (sleep(deadline) != 0){
        /* hopefully the parent has killed */
        return EXIT_SUCCESS;
    }
    for (size_t i = 0; i < num_procs; ++i){
        kill(pids[i], SIGALRM);
    }
    
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[argc+1]){
    int num_procs, deadline, size;
    
    /* array of pids for child processes */
    pid_t* pids;
    pid_t pid;        /* used for the timer */
    
    if (parse_args(&num_procs, &deadline, &size, argc, argv) != EXIT_SUCCESS){
        arg_error();
        return EXIT_FAILURE;
    }

    if ((pids = malloc(num_procs * sizeof(pid_t))) == NULL)
        error_exit("pids malloc error partA4.c\n");

    size_t i = 0;
    do {
        pids[i] = fork();
    } while (pids[i] != 0 && ++i < num_procs);
    /* TODO: Consider changing parent's priority */
    if (pids[i] == 0){
        child_func(size);
    }
    
    /* create the timer process */
    pid = fork();
    if (pid == 0)
        return timer(deadline, num_procs, pids);
    
    /* wait for all the children processes to exit */
    /* less than or equal because +1 for the timer */
    for (size_t i = 0; i <= num_procs; ++i){
        if (waitpid(-1, NULL, 0) == -1){
            printf("Parent wait error\n");
            return EXIT_FAILURE;
        }
        if (i == num_procs - 1){
            kill(pid, SIGKILL);
        }
    }
    
    
    return EXIT_SUCCESS;
}
