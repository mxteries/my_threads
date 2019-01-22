# my_threads
A user-level thread library with similar syntax to pthreads. Created using System V context functions.
I initially wrote this for an assignment for my cs410 class. I ported it here for easy access. 
A matrix multiplication program is included to provide an example of using my_threads.

Threads are scheduled by order of creation (FIFO).
Includes three primary functions:
1. void my_thr_create(int thr_id, void* (*func) (void*), void* arg);
2. void my_thr_start();
3. void my_thr_yield();

A more portable version of this library that does not use System V context functions is currently being worked on
