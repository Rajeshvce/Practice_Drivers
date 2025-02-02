# Workqueues in Linux kernel

A work queue is a mechanism in the Linux kernel that allows deferred execution of tasks in process context. Unlike softirqs and tasklets (which run in interrupt context), work queues allow you to schedule functions to run later in a kernel thread, meaning they can sleep if necessary. This always runs in the process context because workqueue allows users to create a kernel thread and bind work to the kernel thread.

There are two ways of initializing the Workqueue in the Linux kernel
1. Using global workqueue (static/dynamic)
2. Creating own workqueue

## Global Workqueue (Global Worker Thread)

In this method no need to create any workqueue or worker thread. So in this method, we only need to initialize work. We can initialize the work using two methods.
1. Static method
2. Dynamic method

### Static Method

Following is the syntax to create a workqueue using static method:
```
DECLARE_WORK(name, void (*func)(void *))
```
Where,
* name  - The name of the work_struct structure that has to be created
* func  - The function to be scheduled in this workqueue

Example:
```
DECLARE_WORK(workqueue,workqueue_fn);
```

### Dynamic Method

The below call (INT_WORK) creates a workqueue in Linux by the name work and the functions that get scheduled in the queue is work_fn.

```
INIT_WORK(work,work_func);
```
Where,
* name - The name of the "work_struct" structure that has to be created.
* func - The function to be scheduled in this workqueue

### Schedule work to the Workqueue

The below functions are used to allocate the work to the queue

#### Schedule_work

This function is used to queue a work item for execution in the global workqueue (system_wq)
```
int schedule_work(struct work_struct *work)
```
Where,
* work - job to be done

Returns zero if work was already on the kernel-global workqueue and non-zero otherwise.

#### Scheduled_delayed_work

After waiting for a given time this function puts a job in the kernel-global workqueue
```
int scheduled_delayed_work(struct delayed_work *dwork, unsigned long delay);
```
Where,
* dwork - job to be done
* delay - number of jiffies to wait or 0 for immediate execution

#### Schedule_work_on

This puts a job on specific CPU
```
int schedule_work_on(int cpu, struct work_struct *work);
```
Where,
* cpu - CPU to put the work task on
* work - job to be done

#### Scheduled_delayed_work_on

After waiting for a given time this puts a job in the kernel-global workqueue on the specified CPU.
```
int scheduled_delayed_work_on(int cpu, struct delayed_work *dwork, unsigned long delay);
```

### Delete work from workqueue

#### flush_work

This function is used to flush (complete) a specific work item that has been queued on a work queue.When you call flush_work, the kernel will ensure that the specified work item is executed (if it hasn't already started) and will block the caller until the work is fully completed.
```
int flush_work( struct work_struct * work);
```

#### flush_scheduled_work(void)

This function flushes all work items on the kernel-global work queue. It blocks the caller until all work items on the global work queue have been completed.
```
void flush_scheduled_work( void );
```

### Cancel Work from Workqueue

When work is queued on a workqueue, it may not have started execution yet, or it might already be in progress. The kernel provides functions to cancel such work:

#### cancel_work_sync

This function cancels a work item that has been queued on a workqueue. If the work has not yet started execution, it is removed from the queue, and the function returns true (indicating success). If the work is already in progress, the function blocks until the work completes. Use this when you want to ensure that a specific work item is either canceled or fully executed before proceeding. 
```
int cancel_work_sync( struct work_struct *work );
```

#### cancel_delayed_work_sync

This function cancels a delayed work item (a work item scheduled to run after a delay) and works everything similar to above function
```
int cancel_delayed_work_sync( struct delayed_work *dwork );
```

### Check the workqueue

Finally, you can find out whether a work item is pending (not yet executed by the handler) with a call to work_pending or delayed_work_pending.
```
work_pending( work );
delayed_work_pending( work );
```
