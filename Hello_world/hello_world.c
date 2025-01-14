#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>

int val, arrval[4];
char *name;
int cb_val = 0;

module_param(val, int, S_IRUSR | S_IWUSR);
module_param(name, charp, S_IRUSR | S_IWUSR);
module_param_array(arrval, int, NULL, S_IRUSR | S_IWUSR);

int notify_param_change(const char *val, const struct kernel_param *kp){
	int ret = param_set_int(val, kp); //use helper for write variable
	if(ret == 0){
		printk(KERN_INFO "Call back function called\n");
		printk(KERN_INFO "Changed value = %d\n", cb_val);
		return 0;
	}
	return -1;
}

const struct kernel_param_ops my_param_ops = {

	.set = &notify_param_change, // use our setter
	.get = &param_get_int, 	     // using standard getter

};

module_param_cb(cb_val, &my_param_ops, &cb_val, S_IRUGO | S_IWUSR);

static int hello_world_init(void){
	printk("Hello World\n");
	printk(KERN_INFO "val = %d\n", val);
	printk(KERN_INFO "cb_val = %d\n", cb_val);
	printk(KERN_INFO " name = %s\n", name);
	for(int i = 0; i < sizeof(arrval)/sizeof(int);i++){
		printk(KERN_INFO "Arr_val[%d] = %d \n", i,arrval[i]);
	}
	printk(KERN_INFO "Kernel module registered successfully");
	return 0;
}

static void hello_world_exit(void){
	printk("Kernel module unloaded successfully\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Rajesh <rajeshmallempati0112@gmail.com");
MODULE_DESCRIPTION("Hello world Linux driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1:1.0");
