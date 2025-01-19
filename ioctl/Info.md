# Introduction
    Following are the different ways to communicate between Userspace and kernel space
* IOCTL
* Procfs
* Sysfs
* Configfs
* Debugfs
* Sysctl
* UDP Sockets
* Netlink Sockets

## IOCTL in Linux 

* IOCTL is referred to as Input and Output control, which is used to talk to device drivers.
  The major use of this is in case of handling some specific operations of a device which the 
  kernel does not have a system call by default.
* Some realtime Applications of IOCTL are:
    * Ejecting the media from the "cd" driver
    * Changing the baud rate of the Serial port 
    * Adjusting the volume
    * Reading or writing device registers

### Steps involved in IOCTL

* Create IOCTL command in the driver
* Write the IOCTL function in the driver
* Create IOCTL command in the userspace application
* Use the IOCTL system call in the userspace

#### Create IOCTL Command in the Driver

1. Define the IOCTL command
```
#define "ioctl name" __IOX("magic number","command number","argument type")
```
where IOX can be:
    * "IO"  : an ioctl with no parameters
    * "IOW" : an ioctl with write parameters (copy_from_user)
    * "IOR" : an ioctl with read parameters (copy_to_user)
    * "IOWR": an ioctl with both read and write parameters

* The Magic number is a unique number or character that will differentiate our
  set of ioctl calls from the other ioctl calls. some times for the major number is used 
  here 
* Command number is the number that is assigned to the ioctl. This is used to 
  differentiate the commands from the another.
* The last is the type of the data.

2. Add the header file linux/ioctl.h to make use of the above mentioned calls

Example:
```
#include <linux/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
```

#### Write IOCTL function in the driver

* The next step is to implement the IOCTL call we defined into the correspoding driver.
  We need to add the ioctl function to out driver.
```
int  ioctl(struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
```
Where,
    * inode is the inode number of the file being worked on.
    * file is the file pointer to the file that was passed by the application.
    * cmd is the ioctl command that was called from the user space
    * arg are the arguments passed from the userspace

#### Create IOCTL command in the userspace application

    Just define the IOCTL command like how we defined in the driver
```
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
```

#### Use IOCTL System Call in Userspace

* Include the header file <sys/ioctl.h>. Now we need to call the new ioctl command from a user 
  application
```
long ioctl("file descriptor", "ioctl command", "Arguments");
```
Example:
```
ioctl(fd, WR_VALUE, (int32_t*) &number); 
ioctl(fd, RD_VALUE, (int32_t*) &value);
```

