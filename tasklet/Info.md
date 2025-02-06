# Tasklet in Linux Kernel

Tasklets are used to queue up a work to be done at a later time. Tasklets can be run in parallel, but the same tasklet cannot be run on multiple CPUs at the same time. Also each tasklet will run only on the CPU that schedules it, to optimize the cache usage. Since the thread that queued up the tasklet must complete before it can run the tasklet, race conditions are naturally avoided. However, this arrangement can be suboptimal, as other potentially idle CPUs cannot be used to run the tasklet.
In short, a tasklet in linux is something like a very small thread that has neither stack nor context of its own. Such "threads" work quickly and completely.

## Points to remeber

Before using the Tasklets, you should consider the below points.

* Tasklets are atomic, so we cannot use sleep() and such synchronization techniques like semaphores,mutexes,but we can use spinlock.
* A tasklet only runs on the same core (CPU) that schedules it.
* Different tasklets can be running in parallel. But at the same time, a tasklet cannot be called concurrently with itself, as it runs on one CPU only.
* Tasklets are executed by the principle of non-preemptive scheduling, one by one, in turn. We can schedule them with two different priorities: normal and high.

We can create tasklet in two ways:
1. Static method
2. Dynamic method

## Tasklet Structure

This is the important data structure for the tasklet.
```
struct tasklet_struct
{
    struct tasklet_struct *next;  
    unsigned long state;         
    atomic_t count;               
    void (*func)(unsigned long); 
    unsigned long data;           
};
```
Where,
* next  - The next tasklet in line for scheduling
* state - This state denotes Tasklet's State TASKLET_STATE_SCHED (Scheduled) or TASKLET_STATE_RUN (Running)
* count - It holds a nonzero value if the tasklet is disabled and 0 if it is enabled.
* func  - This is the main function of the tasklet. Pointer to the function that needs to schedule for execution at a later time.
* data  - data to be passed to the function "func" 

## Create Tasklet

The below macros used to create a tasklet

### DECLARE_TASKLET

This macro used to create the tasklet structure and assigns the parameters to that structure.
If we are using this macro the tasklet will be in enabled state.
```
DECLARE_TASKLET(name, func, data);
```
Where,
* name  - name of the structure to be created.

Example:
```
DECLARE_TASKLET(tasklet,tasklet_fn, 1);
```

Now we will see how the macro is working. When I call the macro like above, first it creates a tasklet structure with the name of tasklet. Then it assigns the parameter to that structure. It will be looks like below.
```
struct tasklet_struct tasklet = { NULL, 0, 0, tasklet_fn, 1 };

                      (or)

struct tasklet_struct tasklet;
tasklet.next = NULL;
taklet.state = TASKLET_STATE_SCHED;  //Tasklet state is scheduled
tasklet.count = 0;                   //taskelet enabled
tasklet.func = tasklet_fn;           //function
tasklet.data = 1;                    //data arg
```

### DECLARE_TASKLET_DISABLED

The tasklet can be declared and set at a disabled state, which means that the tasklet can be scheduled, but will not run until the tasklet is specifically enabled. You need to use tasklet_enable to enable.
```
DECLARE_TASKLET_DISABLED(name, func, data);
```

## Enable and Disable Tasklet

### tasklet_enable

This is used to enable the tasklet
```
void tasklet_enable(struct tasklet_struct *t);
```
Where,
* t - pointer to the tasklet structure

### tasklet_disable

The function tasklet_disable() is used to prevent a tasklet from executing while ensuring that if it is already running, the function will wait for it to complete before returning.
```
void tasklet_disable(struct tasklet_struct *t);
```

### tasklet_disable_nosync

This is used to disable immediately
```
void tasklet_disable_nosync(struct tasklet_struct *t);
```

NOTE: If the tasklet has been disabled, we can still add it to the queue for scheduling, but it will not be executed on the CPU until it is enabled again. Moreover, if the tasklet has been disabled several times, it should be enabled exactly the same number of times, there is the count field in the structure for this purpose.

## Schedule the Tasklet

When we schedule the tasklet, then that tasklet is placed into one queue out of two, depending on the priority. Queues are organized as singly-linked lists. At that, each CPU has its own queues.

There are two priorities.

1. Normal Priority
2. High Priority

### tasklet_schedule

Schedule a tasklet with a normal priority. If a tasklet has previously been scheduled (but not yet run), the new schedule will be silently discarded.
```
void tasklet_schedule (struct tasklet_struct *t);
```

Example:
```
/*Scheduling Task to Tasklet*/
tasklet_schedule(&tasklet);
```

### tasklet_hi_schedule

Schedule a tasklet with high priority. If a tasklet has previously been scheduled (but not yet run), the new schedule will be silently discarded.
```
void tasklet_hi_schedule (struct tasklet_struct *t);
```

### tasklet_hi_schedule_first

This version avoids touching any other tasklets. Needed for kmemcheck in order not to take any page faults while enqueueing this tasklet. Consider VERY carefully whether you really need this or tasklet_hi_schedule().
```
void tasklet_hi_schedule_first(struct tasklet_struct *t);
```

## Kill Tasklet

Finally, after a tasklet has been created, it’s possible to delete a tasklet through these below functions.

### tasklet_kill

This will wait for its completion and then kill it.
```
void tasklet_kill( struct tasklet_struct *t );
```

Example:
```
/*Kill the Tasklet */
tasklet_kill(&tasklet);
```

### tasklet_kill_immediate

This is used only when a given CPU is in the dead state.
```
void tasklet_kill_immediate( struct tasklet_struct *t, unsigned int cpu );
```
Where,
* cpu - cpu num
