Kernel debugging is generally viewed as an annoying process because it might heavily use printk with static debugging techniques (compile, then run). However, in this work, I want to show you how to write a mprobe module through device interface and then, in user programs, we can use POSIX interface, open()/read()/write(), to operate the virtual device exported through the mprobe module. Through this virtual device, we can observe the interesting kernel information. Therefore, without any help from physical devices, we can dynamically debug developing kernel modules.

In this work, I demonstrated how to write a loadable kernel module and a mprobe module; moreover, the kernel module can be debugged by the mprobe module.
  * https://github.com/HawxChen/Linux-Kernel-Driver-Programming/tree/master/01_mprobe_debugger_and_generanl_driver_module
  * If you feel it useful, please star or watch this repository.

The folder included two modules:
  * A kernel module used hashtable and synchronization.
  * A mprobe module could debug the previous kernel module.
  * Both parts had testing programs and README.


Enjoy tracing the code!
