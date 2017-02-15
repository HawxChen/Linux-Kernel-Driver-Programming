I implemented these Linux Kernel/Driver programs independently.

*   01_mprobe_and_generanl_driver: 
    *   A kernel module uses hashtable and synchronization
    *   A mprobe module can debug the previous kernel module.
    *   Both parts have testing programs and README.

*   02_Concurrent_HC-SR04_miscdeivces:
    *   Two HC-SR04 driver instances registered through miscdevice are equipped with two modes of one-shot and periodical sampling. This kernel module can operate two HC-SR04 sensors in the same time.

*   03_Concurrent_HC-SR04_SYSFS_Platform_Drivers:
    *   This part exports HC-SR04 module to the SYSFS interface as well as Linux Platform Driver interface within PCI mechnaisms.

*   04_barrier_driver_module: 
    *   I implemented the Barrier through kernel driver module. This module should be used through open/read/write/close POSIX intferface.

*   05_barrier_system_call:
    *   I implemented the Barrier through three system calls.
      *  barrier_init(int count, unsigned int\* barrier_id)
      *  barrier_wait(int barrier_id)
      *  barrier_destroy(int barrier_id)
    *   For efficient development and quick debugging, I also provide a driver module (04_barrier_driver_module) in same logic of this synchronzation.
    *   For system call and driver module, I also support testing programs.
