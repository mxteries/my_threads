# my_threads
A user-level thread library with similar syntax to pthreads. Created using System V context functions.
I initially wrote this for an assignment for my cs410 class. I ported it here for easy access.

Threads are scheduled by order of creation (FIFO), so a join() function is not necessary.
Includes three primary functions:
1. void my_thr_create(int thr_id, void* (*func) (void*), void* arg);
2. void my_thr_start();
3. void my_thr_yield();
