#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h> //To make the Major and Minor number to work
#include<linux/kdev_t.h> 
#include<linux/kernel.h>

// Allocate the Major and Minor number statically
// dev_t dev = MKDEV(235, 0);

dev_t dev = 0;

static int hello_world_init(void){
	// Static allocation
	//register_chrdev_region(dev, 1, "Major_Minor");
	
	//Dynamic allocation
	if(alloc_chrdev_region(&dev, 0, 1, "Major_Minor")){
		printk(KERN_INFO "Cannot allocate the major number for the device 1\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d and Minor = %d\n", MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Kernel Module Inserted successfully");
	return 0;
}

static void hello_world_exit(void){
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO " Kernel Modules unloaded successfully");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Rajesh <rajeshmallempati0112@gmail.com");
MODULE_DESCRIPTION(" Driver to understand the Major and Minor Number");
MODULE_LICENSE("GPLv2");
MODULE_VERSION("1:1.0");
