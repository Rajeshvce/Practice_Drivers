#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h> //For Major and Minor Macro functions
#include<linux/kdev_t.h> 
#include<linux/kernel.h>
#include<linux/device.h>

dev_t dev = 0;
static struct class *dev_class;

static int hello_world_init(void){
	
	if(alloc_chrdev_region(&dev, 0, 1, "my_device_driver")){
		printk(KERN_INFO "Cannot allocate the major number for the device 1\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d and Minor = %d\n", MAJOR(dev), MINOR(dev));

	//Creating the struct class
	dev_class = class_create(THIS_MODULE, "my_class");
	if(IS_ERR(dev_class)){
		pr_err("Cannot create the struct class for device\n");
		goto r_class;
	}

	//Creating device
	if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "my_device"))){
		pr_err("Cannot create the device\n");
		goto r_device;
	}
	printk(KERN_INFO "Kernel Module Inserted successfully");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

static void hello_world_exit(void){
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO " Kernel Modules unloaded successfully");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Rajesh <rajeshmallempati0112@gmail.com");
MODULE_DESCRIPTION(" Driver to understand the creation of device files");
MODULE_LICENSE("GPL");
MODULE_VERSION("1:1.0");
