I implemented all Linux Kernel/Driver assignments in ASU CSE530, independently.

*   Assignment01 included two modules: 
    *   A kernel module used hashtable and synchronization
    *   A mprobe module could debug the previous kernel module.
    *   Both parts had testing programs and README.


*   Assignment02 included two parts:
    *   A HC-SR04 driver module registered through miscdevice was equipped with two modes of one-shot and periodical sampling. This kernel module could operate two HC-SR04 sensors in the same time.
    *   Export HC-SR04 module to the SYSFS interface.
    *   Both parts had testing programs and README.

*   Assignment03 is the implementation of synchronization in Linux kernel.
    *   I implemented the Barrier through three system calls.
      *  barrier_init(int count, unsigned int\* barrier_id)
      *  barrier_wait(int barrier_id)
      *  barrier_destroy(int barrier_id)
    *   For efficient development and quick debugging, I also provided a driver module in same logic of this synchronzation.
    *   For system call and driver module, I already supported testing programs.
