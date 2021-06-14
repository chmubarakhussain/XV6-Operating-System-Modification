# XV6-Operating-System-Modofocation
An xv6 process consists of user-space memory (instructions, data, and stack) and per-process state private to the kernel. Xv6 can time-share processes: it transparently switches the available CPUs among the set of processes waiting to execute. When a process is not executing, xv6 saves its CPU registers, restoring them when it next runs the process. The kernel associates a process identifier, or pid, with each process
First of all i found total parallel threads possible on my machchine, 
Then i implemented thread functionality in the code then I created barrier function.
Then at the end added it before the critical section. 
