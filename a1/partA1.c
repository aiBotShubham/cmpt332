/* CMPT 332 -- Fall 2017
* Assignment 1
* Derek Perrin      dmp450 11050915
* Dominic McKeith   dom258 11184543
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "common.h"

#define MAX_THREADS 64
#define MAX_DEADLINE 600
#define MAX_SIZE 200000

DWORD dwTlsIndex; /* enable thread local storage */

/* need CreateThread(), GetSystemTime(), Sleep() */

DWORD WINAPI Thread_Main( LPVOID lpParam ) {
    DWORD init_time;
    size_t counter, size, i;
    /* Get time that thread starts executing
     * Avoid use of GetSystemTime() because Microsoft said so.
     */
    init_time = GetTickCount();
    
    /* set a counter for counting Square() recursive calls */
    counter = 0;
    
    /* Do the following to prevent overwrite and compiler errors
     * for attempting to dereference a LPVOID */
    size = *(size_t*)lpParam;
    
    if(! TlsSetValue(dwTlsIndex, &counter))
        error_exit("TlsSetValue error in Thread_Main\n");
    
    for ( i = 1; i <= size && keepRunning; ++i){
        Square(i);
    }
    
    counter = *(size_t*)TlsGetValue(dwTlsIndex);
    printf("Thread %d: No. of Square calls: %lu, Elapsed Time: %d ms\n",
            GetCurrentThreadId(),
            counter,
            GetTickCount() - init_time);
    return EXIT_SUCCESS;
}

void incr_func(){
    size_t* counter;
    counter = TlsGetValue(dwTlsIndex);
    ++(*counter);
}

/* Error function to return errors to stderr */
VOID error_exit (LPSTR lpszMessage) {
    fprintf(stderr, "%s %d\n", lpszMessage, GetLastError());
    ExitProcess(0);
    
    
}

int main(int argc, char* argv[]){
    HANDLE* thread_array;
    int num_threads, size, deadline;
    size_t i;

    if ( parse_args(&num_threads, &deadline, &size, argc, argv) != 0 ) {
        arg_error();
        return EXIT_FAILURE;
    }

    /* check that arguments don't exceed maximum values */
    if (num_threads > MAX_THREADS)
        error_exit("Error: Maximum number of threads arg too high. "
                "MAX_THREADS = 64\n");
    if (deadline > MAX_DEADLINE)
        error_exit("Error: Maximum deadline arg too high. "
                "MAX_DEADLINE = 600 s\n");
    if (size > MAX_SIZE)
        error_exit("Error: Maximum size arg is too high. "
                "MAX_SIZE = 200000\n");


    /* create space for the threads */
    thread_array = malloc(sizeof(HANDLE) * num_threads);
    if (thread_array == NULL){
        error_exit("thread_array malloc error\n");
    }
    
    /* allocating a TLS index */
    if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) 
        error_exit("TlsAlloc error\n");
    
    /* create threads and stash them in thread array */
    printf("Creating threads\n");
    for ( i = 0; i < num_threads; ++i){
        thread_array[i] = CreateThread(
                NULL,          /* security attributes */
                2<<22,         /* Stack size that accommodates size=200000 */
                Thread_Main,   /* thread function */
                &size,         /* ptr to thread function argument */
                0,             /* default creation flag */
                NULL);         /* thread identifier */
    }
    Sleep(1000*deadline); /* 1000 for converting s to ms */
    keepRunning = false;

    /* Wait until threads complete before exiting main() */
    if (WaitForMultipleObjects(
            num_threads,                 /* number of threads in array */
            thread_array,            /* pointer to array of object handles */
            TRUE,                /* bWaitAll: TRUE to wait for all threads */
            INFINITE) == WAIT_FAILED){   /* wait until all threads return */
        error_exit("WaitForMultipleObjects error.\n");
    }
    
    /* free allocated memory */

    TlsFree(dwTlsIndex);        /* free allocated TLS memory */
    free(thread_array);         /* free thread array */  

    return EXIT_SUCCESS;
}
