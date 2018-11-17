#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// size of stack for each thread
#define STACK_SIZE 8192

/*                  global variables to be accessed by each thread                  */

ucontext_t main_thread; // main thread
ucontext_t exit_context; // linked by every thread to "wrap up"
ucontext_t* uc; // uc is essentially a queue. uc[0] is always the active thread!!
int uc_size = 0;
int num_threads = 0; // number of contexts to init
int active_threads = 0;

void my_thr_create(void (*func) (int), int thr_id);
void my_thr_start();
void my_thr_yield();
void _exit_thread();

/**
 * my_thr_create initializes the necessary context for each thread,
 * this function also schedules each thread (FCFS), and
 * we set up each thread to start on a function that takes in an int when activated
 */
void my_thr_create(void (*func) (int), int thr_id) {
    num_threads++;
    active_threads++;

    // dynamically allocate thread contexts (aka uc) using realloc
    if (num_threads >= uc_size) {
        uc_size = (uc_size + 1) * 2;
        uc = realloc(uc, uc_size * sizeof(*uc));
        if (uc == NULL) {
            fprintf(stderr, "uc malloc/realloc failed\n");
        }
    }

    // initialize an exit context that every context points to
    getcontext(&exit_context);
    exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    if (exit_context.uc_stack.ss_sp == NULL) {
        fprintf(stderr, "uc stack malloc failed\n");
    }
    exit_context.uc_stack.ss_size = STACK_SIZE;
    exit_context.uc_link = &main_thread;
    makecontext (&exit_context, (void (*) (void)) _exit_thread, 0);


    // initialize the context for this thread
    getcontext(&uc[thr_id]);
    uc[thr_id].uc_stack.ss_sp = malloc(STACK_SIZE);
    if (uc[thr_id].uc_stack.ss_sp == NULL) {
        fprintf(stderr, "uc stack malloc failed\n");
    }
    uc[thr_id].uc_stack.ss_size = STACK_SIZE;
    // every thread goes into exit_context when finished
    uc[thr_id].uc_link = &exit_context;

    makecontext (&uc[thr_id], (void (*) (void)) func, 1, thr_id);
}


// starts the first thread by switching from main
void my_thr_start() {
    swapcontext(&main_thread, uc);
}

// rewrites the current thread in the uc array
// schedules the next thread in line.
// if active threads is 0, returns to main
void _exit_thread() {

    active_threads--;
    free(uc[0].uc_stack.ss_sp);
    for (int i = 0; i < active_threads; i++) {
        uc[i] = uc[i+1]; // delete the current context from the running queue
    }
    if (active_threads > 0) {
        setcontext(&uc[0]); // set the next scheduled thread in motion
    }
    // else return to the main thread (linked context)
}

// pauses execution of current thread and cooperatively yields
// to next scheduled thread. current thread gets put at end of queue (uc).
// if there is only 1 active thread, my_thr_yield does not do anything
void my_thr_yield() {
    if (active_threads <= 1) {
        return;
    }
    // save the context of the current thread in this temp var
    ucontext_t current_thread = uc[0];

    // schedule the current thread at the end of the queue
    for (int i = 0; i < active_threads - 1; i++) {
        uc[i] = uc[i+1];
    }
    uc[active_threads - 1] = current_thread;

    // rewrite the old thread's context so it can resume later.
    // start the next scheduled thread
    swapcontext(&uc[active_threads - 1], &uc[0]);
}