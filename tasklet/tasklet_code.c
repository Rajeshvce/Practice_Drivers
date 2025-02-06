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
#include<linux/kobject.h>
#include<linux/interrupt.h>
#include<asm/io.h>

#define IRQ_NO 11

void tasklet_fn(unsigned long);

//DECLARE_TASKLET(tasklet, tasklet_fn);
/* Tasklet by Dynamic Method */
struct tasklet_struct *tasklet = NULL;


void tasklet_fn(unsigned long arg)
{
        printk(KERN_INFO "Executing tasklet Function: arg = %ld\n", arg);
}

//Interrupt hander for IRQ11
static irqreturn_t irq_handler(int irq, void *dev_id){
	pr_info("Shared IRQ: Interrupt occured\n");
	tasklet_schedule(tasklet);

	return IRQ_HANDLED;
}

volatile int my_val = 0;  //value used in sysfs

dev_t dev = 0;
static struct cdev my_cdev;
static struct class *dev_class;
static struct kobject *kobj_ref;

/*************************** Driver functions ******************************/
static int driver_open(struct inode* inode, struct file *file);
static int driver_release(struct inode* inode, struct file *file);
static ssize_t driver_read(struct file * filp, char __user * buf, size_t len, loff_t * off);
static ssize_t driver_write(struct file * filp, const char * buf, size_t len, loff_t * off);

/*************************** Sysfs *****************************************/
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);
struct kobj_attribute my_attr = __ATTR(my_val, 0660, sysfs_show, sysfs_store);

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
        asm("int $0x3B");  // Corresponding to irq 11
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

/*
** This function will be called when we read the sysfs file
*/
static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
        pr_info("Sysfs - Read!!!\n");
        return sprintf(buf, "%d", my_val);
}
/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count)
{
        pr_info("Sysfs - Write!!!\n");
        sscanf(buf,"%d",&my_val);
        return count;
}

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
	
	/* Creating a directory in the /sys/kernel using the kobject */
	kobj_ref = kobject_create_and_add("my_sysfs",kernel_kobj);

	/* Create the sysfs file for my_value */
	if(sysfs_create_file(kobj_ref, &my_attr.attr)){
		pr_err("Cannot create the sysfs file\n");
		goto r_sysfs;
	}

	if(request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "my_device", (void *)(irq_handler))){
		pr_info(" my_device: cannot register IRQ\n");
		goto irq;
	}
	
	/* Init the tasklet bt Dynamic Method */
        tasklet  = kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
        if(tasklet == NULL) {
            printk(KERN_INFO "etx_device: cannot allocate Memory");
            goto irq;
        }
        tasklet_init(tasklet,tasklet_fn,0);

	pr_info("Device driver loaded successfully\n");
	return 0;

irq:
	free_irq(IRQ_NO, (void *)(irq_handler));
r_sysfs:
	sysfs_remove_file(kernel_kobj, &my_attr.attr);
	kobject_put(kobj_ref);
r_device:
	class_destroy(dev_class);
r_class:
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	return -1;
}	

static void __exit my_module_exit(void){
	tasklet_kill(tasklet);

	if(tasklet != NULL)
        {
          kfree(tasklet);
        }
	free_irq(IRQ_NO, (void *)(irq_handler));
	sysfs_remove_file(kernel_kobj, &my_attr.attr);
	kobject_put(kobj_ref);
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
