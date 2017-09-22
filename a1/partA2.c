/* CMPT 332 -- Fall 2017
* Assignment 1
* Derek Perrin 		dmp450 11050915
* Dominic McKeith 	dom258 11184543
*/

#include <stdio.h>
#include <stdlib.h>
#include <os.h>

#include "common.h"

#define MAX_DEADLINE 600
#define MAX_THREADS 1000
#define MAX_SIZE 500000

size_t* square_counts;

void incr_func(void){
	square_counts[MyPid() - 2]++;
}

void child_main(int* n) {
	long start_time = Time();
    int size = *n;
    for (size_t i = 1; i <= size; ++i) {
        Square(i);
    }
	PID my_pid = MyPid();
	
	long run_time = (Time() - start_time)*10;	/* 10 is for us to ms */
    printf("Thread %d: No. of Square calls: %lu, Elapsed time: %lu ms\n",
		my_pid, square_counts[my_pid - 2], run_time);
    Pexit();
}

void parent_main(int* args){
    /* declare necessary argument variables */
    int num_threads = args[0];
    int deadline = args[1];
	
	square_counts = malloc(sizeof(size_t) * num_threads);

    PID* thread_array;
    thread_array = malloc(sizeof(PID) * num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        /* create thread */
		square_counts[i] = 0;
        thread_array[i] = Create(
                child_main, /* pointer to child thread function */
                2 << 22,    /* stack size */
                "child",    /* look at string cat, or if we need unique */
                args + 2,       /* pointer to arguments for child_main */
                NORM,       /* NORM privilege level */
                USR        /* user level thread */
          );
        if (thread_array[i] == PNUL){
            error_exit("Create child thread error\n");
        }
    }
    /* Sleep parent thread until deadline */
    Sleep(deadline * 100);  /* TICKINTERVAL = 10000 micro-s per tick */
    
	/* deadline * 1000 is for seconds -> milliseconds */
    for (size_t i = 2; i < num_threads + 2; ++i) {
        if (PExists(i)){
            if (Kill(i) == PNUL)
                error_exit("Kill thread error\n");
            printf("Thread %lu: No. of Square calls:"
                    " %lu, Elapsed time: %d ms\n",
                    i, square_counts[i - 2], deadline * 1000);
        }
    }
    

    free(thread_array);
    free(args);
	free(square_counts);

    /* exit */
    Pexit();
}

void error_exit(char* error_message){
    fprintf(stderr, "%s", error_message);
    exit(EXIT_FAILURE);
}

int mainp(int argc, char* argv[argc+1]){
    int* args;
    args = malloc(sizeof(int) * (argc - 1));

    /* parse arguments. I'm abusing pointer notation a bit */
    if (parse_args(args, args+1, args+2, argc, argv) != 0 ){
        arg_error();
        exit(EXIT_FAILURE);
    }
	
    if(Create(
            parent_main,            /* function pointer to thread func */
            2<<22,                  /* stack size magic number */
            "parent_thread",        /* name identifier of thread */
            args,                   /* argument pointer */
            HIGH,                   /* HIGH because it's the parent */
            USR)                    /* user level threads */ 
        == PNUL) {
        error_exit("Create parent error\n");
    }

	printf("Got to procedure main A2\n");
	return EXIT_SUCCESS;
}
