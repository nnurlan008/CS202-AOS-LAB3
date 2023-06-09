#include "kernel/spinlock.h" // Contains the definitions of spinlock functions
#include "kernel/types.h" // Contains the definitions of uint
#include "user/thread.h" //  Contains the definitions of struct lock_t* lock
#include "user/user.h" // Contains the definition of malloc
#define PGSIZE 4096 // default page size

// Creating a thread in a similar way to fork()
int thread_create(void *(start_routine)(void*), void *arg) {

  //allocating a stack_ptr of PGSIZE bytes = 4096
  int pointer_size = PGSIZE*sizeof(void);
  void* stack_pointer = (void* )malloc(pointer_size);
  int thread_id = clone(stack_pointer); // similar to 

  // thread id = 0 for child thread, so, the below if condition is calling
  // start_routine for child thread.
  if (thread_id == 0) {
    (*start_routine)(arg);
    exit(0);
  }

  // this will return 0 to parent process
  return 0;
}

// Initializing the lock
void lock_init(struct lock_t* lock) {
  lock->locked = 0;
}

void lock_acquire(struct lock_t* lock) {
    // this function is acquiring the lock for the calling thread
    // __sync_lock_test_and_set is an atomic cas operation 
    // trying to make the lock=1; acquiring the lock
    // then __sync_synchronize() is called to synchronize 
    // all the moads and stores between CPUs
    while(__sync_lock_test_and_set(&lock->locked, 1) != 0);
    __sync_synchronize();
}

void lock_release(struct lock_t* lock) {

    // First, __sync_synchronize() is called to synchronize 
    // all the moads and stores between CPUs
    __sync_synchronize();

    // release lock after being done with the critical operation
    // lock=0 to release the lock
    __sync_lock_release(&lock->locked, 0);
//
}