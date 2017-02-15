*   This work includes the implementation of synchronization in Linux kernel.
    *   I implemented the Barrier through three system calls.
      *  barrier_init(int count, unsigned int\* barrier_id)
      *  barrier_wait(int barrier_id)
      *  barrier_destroy(int barrier_id)
    *   For efficient development and quick debugging, I also provide a driver module (04_barrier_driver_module) in same logic of this synchronzation.

    *   For system call and driver module, I also support testing programs.
#For kernel patch
1. copy "patch.file" into your Kernel Source directory.
2. patch -p1 < patch.file
3. build & install

#For testing
1. go to assignment3/official/barrier_test
2. make
3. execute ./barrier_user_sys on Gne2 Board
   ./barrier_user_sys [microseconds]
