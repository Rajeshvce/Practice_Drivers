#include<linux/init.h>
#include<linux/module.h>

static int hello_world_init(void){
	printk("Hello World\n");
	return 0;
}

static void hello_world_exit(void){
	printk("Leaving the world\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Rajesh <rajeshmallempati0112@gmail.com");
MODULE_DESCRIPTION("Hello world Linux driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1:1.0");
