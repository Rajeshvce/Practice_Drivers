# Cdev Structure

* In Linux kernel struct inode structure is used to represent files. Therefore
it is different from the file structure that represents an open file descriptor
* struct cdev is one of the elements of the elements of the inode structure. This 
field contains a pointer to that structure when the inode refers to a char device 
file.
```
struct cdev { 
    struct kobject kobj; 
    struct module *owner; 
    const struct file_operations *ops; 
    struct list_head list; 
    dev_t dev; 
    unsigned int count; 
};
```

* In cdev structure we need to fill the two fields
    * file_operation
    * owner (This should be THIS_MODULE)

* There are two ways of allocating and initializing one of these structures
1. Runtime allocation
2. Own allocation

* If you wish to obtain a standalone cdev structure at runtime, you may do so with 
code such as 
```
struct cdev *my_cdev = cdev_alloc( );
my_cdev->ops = &my_fops;
```

* or else you can embed the cdev structure within a device-specific structure of
your own by using the below function
```
void cdev_init(struct cdev *cdev, struct file_operations *fops);
```

* Once the cdev structure is set up with file_operations and owner, the final 
step is to tell the kernel about it with a call to:
```
int cdev_add(struct cdev *dev, dev_t num, unsigned int count);
```
where 
dev is the cdev structure
num is the first device number to which this device responds, and
count is the number of device numbers that should be associated with the device.

* To remove a char device from the system call:
```
void cdev_del(struct cdev *dev);
```

# File_operations

* The file operations structure is how a char device sets up this connection. The
  structure, (defined in <linux/fs.h>), is a collection of function pointers. Each
  open file is associated with its own set of functions. The operations are mostly
  in charge of implementing the system calls and are, therefore, named open, read, and so on.

* A file_operations structure is called fops. Each field in the structure must point to the
  function in the driver that implements a specific operation or have to left NULL for unsupported
  operations. The whole structure is mentioned below snippet.
```
struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
    ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
    int (*iterate) (struct file *, struct dir_context *);
    int (*iterate_shared) (struct file *, struct dir_context *);
    unsigned int (*poll) (struct file *, struct poll_table_struct *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *, fl_owner_t id);
    int (*release) (struct inode *, struct file *);
    int (*fsync) (struct file *, loff_t, loff_t, int datasync);
    int (*fasync) (int, struct file *, int);
    int (*lock) (struct file *, int, struct file_lock *);
    ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
    unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
    int (*check_flags)(int);
    int (*flock) (struct file *, int, struct file_lock *);
    ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
    ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
    int (*setlease)(struct file *, long, struct file_lock **, void **);
    long (*fallocate)(struct file *file, int mode, loff_t offset,
              loff_t len);
    void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
    unsigned (*mmap_capabilities)(struct file *);
#endif
    ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
            loff_t, size_t, unsigned int);
    int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
            u64);
    ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
            u64);
};
```

## Some important functions

### kmalloc

* kmalloc function is used to allocate the memory in kernel space 
```
#include <linux/slab.h>

void *kmalloc(size_t size, gfp_t flags);
```

Arguments:
* size  - how many bytes of memory are required
* flags - The type of memory to allocate
    * GFP_USER – Allocate memory on behalf of the user. May sleep.
    * GFP_KERNEL – Allocate normal kernel ram. May sleep.
    * GFP_ATOMIC – Allocation will not sleep. May use emergency pools. For example, use this inside interrupt handler.
    * GFP_HIGHUSER – Allocate pages from high memory.
    * GFP_NOIO – Do not do any I/O at all while trying to get memory.
    * GFP_NOFS – Do not make any fs calls while trying to get memory.
    * GFP_NOWAIT – Allocation will not sleep.
    * __GFP_THISNODE – Allocate node-local memory only.
    * GFP_DMA – Allocation is suitable for DMA. Should only be used for kmalloc caches. Otherwise, use a slab created with SLAB_DMA.
    * __GFP_COLD – Request cache-cold pages instead of trying to return cache-warm pages.
    * __GFP_HIGH – This allocation has high priority and may use emergency pools.
    * __GFP_NOFAIL – Indicate that this allocation is in no way allowed to fail (think twice before using).
    * __GFP_NORETRY – If memory is not immediately available, then give up at once.
    * __GFP_NOWARN – If allocation fails, don’t issue any warnings.
    * __GFP_REPEAT – If allocation fails initially, try once more before failing.

### kfree()

* This is like a free() function in the userspace. This is used to free the previously allocated memory.
```
void kfree(const void *objp)
```
Arguments:
* *objp - pointer returned by kmalloc

### copy_from_user()

* This function is used to Copy a block of data from user space (Copy data from kernel space to user space)
```
unsigned long copy_to_user(const void __user *to, const void *from, unsigned long n);
```
Arguments:
* to   - Destination address, in the user space
* from - The source address in the kernel space
* n    - Number of bytes to copy 

Returns  number of bytes that could not be copied. On success, this will be zero

### copy_to_user()

* The function is used to Copy a block of data into userspace (Copy data from kernel 
  space to user space)
```
unsigned long copy_to_user(const void __user *to, const void *from, unsigned long  n);
```
Arguments:
* to   - Destination address, in the user space
* from - The source address in the kernel space
* n    - Number of bytes to copy

Returns number of bytes that could not be copied. On success, this will be zero   
