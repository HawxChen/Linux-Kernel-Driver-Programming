I implemented all Linux Kernel/Driver assignments in ASU CSE530, independently.

*   Assignment01 includes two modules: 
    *   A kernel module uses hashtable and synchronization
    *   A mprobe module can debug the previous kernel module.
    *   Both parts have testing programs and README.

*   Assignment02 includes two parts:
    *   Two HC-SR04 driver instances registered through miscdevice are equipped with two modes of one-shot and periodical sampling. This kernel module can operate two HC-SR04 sensors in the same time.
    *   The part2 exports HC-SR04 module to the SYSFS interface.
    *   Both parts have testing programs and README.

*   Assignment03 includes the implementation of synchronization in Linux kernel.
    *   I implemented the Barrier through three system calls.
      *  barrier_init(int count, unsigned int\* barrier_id)
      *  barrier_wait(int barrier_id)
      *  barrier_destroy(int barrier_id)
    *   For efficient development and quick debugging, I also provide a driver module in same logic of this synchronzation.
    *   For system call and driver module, I also support testing programs.
