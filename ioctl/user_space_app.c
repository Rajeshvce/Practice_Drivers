#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include<sys/types.h>

#define WRITE_DRV _IOW('a','a',int32_t*)
#define READ_DRV _IOR('a','b',int32_t*)

int main(){
	int fd;
	int32_t value, number;

	fd = open("/dev/my_device", O_RDWR);
	if(fd < 0)
	{
		printf("Unable to open the device\n");
		return 0;
	}
	
	printf("Enter the Value to send\n");
        scanf("%d",&number);
        printf("Writing Value to Driver\n");
        ioctl(fd, WRITE_DRV, (int32_t*) &number); 
 	
        printf("Reading Value from Driver\n");
        ioctl(fd, READ_DRV, (int32_t*) &value);
        printf("Value is %d\n", value);
 
        printf("Closing Driver\n");
        close(fd);
	return 0;
}
