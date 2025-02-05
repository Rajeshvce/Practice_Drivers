# Kernel Thread

## Process

An executing instance of a program is called process. 

## Thread

A thread is a single sequence stream within a process. Thread is an independent flow of control that operates within the same address space as other independent flows of control within a process. One process can have multiple threads, with each thread executing different code concurrently while sharing data and synchronizing much more easily than cooperating processes.

Thread require fewer system resources than processes and can start more quickly. Threads, are also known as light weight processes.

One of the advantages of the threads is that since all the threads within the processes share the common address space, the communication between the threads is far easier and less time consuming as compared to processes.

This approach has one disadvantage also. It leads to several concurrency issues and requires synchronization mechanisms to handle the same.

## Thread Management

Whenever we are creating a thread, it has to be managed by someone. So management follows like below.
* A thread is a seqeunce of instructions
* CPU can handle one instruction at a time.
* To switch between instructions on parallel threads, the execution state needs to be saved.
* Execution state in its simplest form is a program counter and CPU registers.
* The program counter tells us what instruction to execute next.
* CPU registers hold execution arguments, for example, additional operands.
* This alteration between the threads require management.
* Management includes saving state, restoring state, and deciding what thread to pick next.

## Types of threads

There are two types of thread.

1. User Level Thread
2. Kernel Level Thread

### User Level Thread

In this type, the kernel is not aware of these threads, Everything is maintained by the user thread library. That thread library contains code for creating and destroying threads, for passing messages and data between the threads, for scheduling thread execution, and for saving and restoring thread contexts. So all will be in User Space.

### Kernel Level Thread

Kernel threads are managed by the OS, therefore, thread operations are implemented in the kernel code. There is no thread management code in the application area.

## Kernel Thread Management Functions

There are many functions used in Kernel Thread. They are

* Create Kernel Thread
* Start Kernel Thread
* Stop Kernel Thread
* Other functions in Kernel Thread

To use the above functions we should include linux/kthread.h header file.

### Create Kernel Thread

#### kthread_create

```
struct task_struct * kthread_create (int (* threadfn(void *data), void *data, const char namefmt[], ...);
```

Where,
* threadfn  - the function to run until signal_pending(current)
* data      - data pointer for threadfn
* namefmt[] - printf-style named for the thread.

This helper function creates and names a kernel thread. But we need to wakeup that thread manually. When woken, the thread will run threadfn() with data as its argument.
threadfn can either call do_exit directly if it is a standalone thread for which no one will call kthread_stop, or return when 'kthread_should_stop' is true (which means kthread_stop has been called). The return value should be zero or a negative error number, it will be passed to kthread_stop.

It returns task_struct or ERR_PTR(-ENOMEM)

### Start Kernel Thread

#### waku_up_process

This is used to wakeup a specific process.
```
int wake_up_process (struct task_struct * p);
```
Where,
* p   - The process to be woken up.

Attempt to wakeup the the nominated process and move it to the set of runnable processes. It return 1 if the process is woken up, 0 if it was already running.
It may be assumed that this function implies a write memory barrier before changing the task state if and only if any tasks are woken.

### Stop Kernel Thread

#### kthread_stop

It stops a thread created by kthread_create.
```
int kthread_stop ( struct task_struct *k);
```
Where,
* k  - kthread created by kthread_create

Sets kthread_should_stop for k to return true, wakes it and waits for it to exit. Your threadfn must not call do_exit itself, if you use this function! This can also be called after kthread_create instead of calling wake_up_process: the thread will exit without calling threadfn.

It Returns the result of threadfn, or –EINTR if wake_up_process was never called.

### Other Functions in Kernel Thread

#### kthread_should_stop

```
int kthread_should_stop (void);
```

This is a Linux kernel function that determines if a kernel thread should return immediately. When kthread_stop() is called on a kernel thread, kthread_should_stop() will return true. The kernel thread should then return, and its return value will be passed to kthread_stop().

#### kthread_bind

This is used to bind a just-created kthread to a CPU.
```
void kthread_bind (struct task_struct *k, unsigned int cpu);
```
Where,
* k  - thread created by kthread_create.
* cpu - CPU (might not be online, must be possible) for k to run on.

### Implementation

#### Thread function

First, we have to create our thread that has the argument of void * and should return int value. We should follow some conditions in our thread functions. It is advisable
* If that thread is a long-run thread, we need to check kthread_should_stop() every time, because any function may call kthread_stop. If any function called kthread_stop, that time kthread_should_stop will return true. We have to exit our thread function if true value has returned by kthread_should_stop.
* But if your thread function is not long, then let that thread finish its task and kill itself using do_exit.

In my thread function, let's print something every minute and it is a continuous process. So let's check the kthread_should_stop every time. See the below snippet to understand.
```
int thread_function(void *pv) 
{
 int i=0;
 while(!kthread_should_stop())
 {
   printk(KERN_INFO "In EmbeTronicX Thread Function %d\n", i++);
   msleep(1000);
 } 
 return 0; 
}
```

#### Creating and Starting Kernel Thread

So as of know, we have our thread function to run. Now, we will create a kernel thread using kthread_create and start the kernel thread using wake_up_process.
```
static struct task_struct *my_thread;

my_thread = kthread_create(thread_function,NULL,"my_thread");
if(my_thread){
    wake_up_process(my_thread);
}
else{
    pr_err("Cannot create kthread \n");
}
```

There is another function that does both processes (create and start). That is kthread_run(). You can replace both kthread_create and wake_up_process using this function.

##### kthread_run

This is used to create and wake a thread.
```
kthread_run(threadfn, data, namefmt,....)
```
Convenient wrapper for kthread_create followed by wake_up_process. It returns the kthread or ERR_PTR (-ENOMEM).

You can see the below snippet which using kthread_run.
```
static struct task_struct *etx_thread;

etx_thread = kthread_run(thread_function,NULL,"eTx Thread"); 
if(etx_thread) 
{
 printk(KERN_ERR "Kthread Created Successfully...\n");
}
else 
{
 printk(KERN_ERR "Cannot create kthread\n"); 
}
```

##### kthread_stop

You can stop the kerne thread using kthread_stop. Use the below snippet to stop.
```
kthread_stop(etx_thread);
```

