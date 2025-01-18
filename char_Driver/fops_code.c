#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>
#include<linux/err.h>
#include<linux/device.h>
#include<linux/device.h>

dev_t dev = 0;
static struct class *cdev_class;
static struct cdev my_cdev;

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
	pr_info("Read function is called from the device\n");
	return 0;
}

static ssize_t fops_write(struct file* filp, const char __user *buf, size_t len, loff_t *off)
{
	pr_info("Write function is called from the device\n");
	return len;
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
	
	pr_info("Driver loaded successfully\n");
	return 0;	

r_device:
	class_destroy(cdev_class);

r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

static void fops_exit(void){
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
