#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>
#include<linux/err.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define mem_size 1024

dev_t dev = 0;
static struct class *cdev_class;
static struct cdev my_cdev;
uint8_t *kernel_buffer;

static int fops_open(struct inode *inode, struct file *file){
	pr_info("Device opened successfully\n");
	return 0;
}

static int fops_close(struct inode *inode, struct file *file){
	pr_info("Device closed successfully\n");
	return 0;
}

static ssize_t fops_read(struct file* filp, char __user *buf, size_t len, loff_t *off)
{
    int nbytes;               /* Number of bytes read */
    int bytes_to_do;          /* Number of bytes to read */
    int data_size_remaining;  /* Remaining data size in buffer */

    data_size_remaining = mem_size - *off;  // Read only valid written data

    /* If no more data is available, return 0 (EOF) */
    if (data_size_remaining <= 0) {
        pr_info("Reached end of device: returning EOF\n");
        return 0;
    }

    /* Determine how many bytes to read */
    bytes_to_do = (data_size_remaining > len) ? len : data_size_remaining;

    /* Copy data from kernel space to user space */
    nbytes = bytes_to_do - copy_to_user(buf, kernel_buffer + *off, bytes_to_do);

    *off += nbytes; // Update file offset
    pr_info("Data read: %d bytes\n", nbytes);

    return nbytes;

}


static ssize_t fops_write(struct file* filp, const char __user *buf, size_t len, loff_t *off)
{
	int nbytes; /* No of bytes written */

	if(len > mem_size){
		pr_warn("Write truncated: Buffer size exceeded\n");
	        len = mem_size;
	}

	memset(kernel_buffer, 0, mem_size);
	nbytes = len - copy_from_user(kernel_buffer, buf, len);
	
	pr_info("Data write: Done\n");
	*off += nbytes;
	return nbytes;
}

struct file_operations fops = {
	.owner   = THIS_MODULE,
	.read    = fops_read,
	.write   = fops_write,
	.open    = fops_open,
	.release = fops_close,
};

static int fops_init(void){
	if((alloc_chrdev_region(&dev, 0, 1, "my_dev"))){
		pr_err("Cannot allocate the major number");
		return -1;
	}

	pr_info("Major = %d and Minor = %d\n", MAJOR(dev), MINOR(dev));

	cdev_init(&my_cdev, &fops);

	if(cdev_add(&my_cdev, dev, 1)){
		pr_err("Cannot add the device to the system\n");
		goto r_class;
	}
	
	/* Creating the class to create a device node in sysfs entry*/

	if(IS_ERR(cdev_class = class_create(THIS_MODULE, "my_class"))){
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}

	/* IN above we have created the class now create the device under this class*/
	
	if(IS_ERR(device_create(cdev_class, NULL, dev, NULL, "my_device"))){
		pr_err("Cannot create the device\n");
		goto r_device;
	}

	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0){
		pr_info("Cannot allocate memory in kernel\n");
		goto r_device;
	}

	strcpy(kernel_buffer, "Hello_world\n");

	pr_info("Driver loaded successfully\n");
	return 0;	

r_device:
	class_destroy(cdev_class);

r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

static void fops_exit(void){
	kfree(kernel_buffer);
	device_destroy(cdev_class, dev);
	class_destroy(cdev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev,1);
	pr_info("Driver unloaded successfully\n");
}

module_init(fops_init);
module_exit(fops_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rajesh Mallempati <rajeshmallempati9140@gmail.com");
MODULE_DESCRIPTION("Driver to understand the fops operations");
MODULE_VERSION("1:1.0");
