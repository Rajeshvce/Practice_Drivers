#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sysfs.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/err.h>
#include<linux/kthread.h>
#include<linux/delay.h>
#include<linux/sched.h> // for task_struct

dev_t dev = 0;
static struct cdev my_cdev;
static struct class *dev_class;

static struct task_struct *my_thread;

/*************************** Driver functions ******************************/
static int driver_open(struct inode* inode, struct file *file);
static int driver_release(struct inode* inode, struct file *file);
static ssize_t driver_read(struct file * filp, char __user * buf, size_t len, loff_t * off);
static ssize_t driver_write(struct file * filp, const char * buf, size_t len, loff_t * off);

/*****************************************************************************/

int thread_function(void *pv){
	int i = 0;
	while(!kthread_should_stop()){
		pr_info("In Thread function %d\n",i++);
		msleep(1000);
	}
	return 0;
}

static int driver_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/ 
static int driver_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}
 
/*
** This function will be called when we read the Device file
*/
static ssize_t driver_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read function\n");
        return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t driver_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write Function\n");
        return len;
}

/* File operations structure */
static struct file_operations fops = {
	.owner     = THIS_MODULE,
	.open      = driver_open,
	.read      = driver_read,
	.write     = driver_write,
	.release   = driver_release,
};

static int __init my_module_init(void){
	/* allocate major and minor number */
	/* Return value 0 on success and non-negative on failure */
	if((alloc_chrdev_region(&dev, 0, 1, "my_char_device")) <0){
		pr_err("Cannot allocate major number\n");
		return -1;
	}

	pr_info("Major number=%d and Minor number=%d\n", MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&my_cdev, &fops);

	/* Adding character device to the system */
	if((cdev_add(&my_cdev,dev,1)) <0){
			pr_err("Unable to add the device to the system\n");
			goto r_class;
	}

	/* Creating struct class */
	if(IS_ERR(dev_class = class_create(THIS_MODULE, "my_class"))){
			pr_err("Cannot create the class\n");
			goto r_class;
	}
	
	/* Creating device for /dev */
	if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "my_device"))){
			pr_err("Cannot create the device\n");
			goto r_device;
	}
	
	/* Create the kthread */
	my_thread = kthread_create(thread_function, NULL, "My Thread");
	if(my_thread){
		wake_up_process(my_thread);
	}
	else{
		pr_err("Unable to create the kthread\n");
		goto r_device;
	}
		
	pr_info("Device driver loaded successfully\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	return -1;
}	

static void __exit my_module_exit(void){
	
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Device driver unloaded successfully\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(" Mallempati Rajesh <rajeshmallempati0112@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (sysfs)");
MODULE_VERSION("1.0");
