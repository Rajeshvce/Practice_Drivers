# Sysfs

## Kernel objects

The heart of the sysfs model is the kernel object. Kobject is the glue that binds the 
sysfs and the kernel, which is represented by struct kobject and defined in linux/kobject.h
A struct object represents a kernel object, maybe a device or so, such as the things that
show up as a directory in the sysfs file system.

Kobjects are usually embedded in other structures
```
#define KOBJ_NAME_LEN 20 

struct kobject {
 char *k_name;
 char name[KOBJ_NAME_LEN];
 struct kref kref;
 struct list_head entry;
 struct kobject *parent;
 struct kset *kset;
 struct kobj_type *ktype;
 struct dentry *dentry;
};
```
Where,
* name   - Name of the kobject. Current kobject is created with this name in sysfs
* parent - This is kobject's parent.
* ktype  - the type associated with the kobject
* kset   - a group of kobjects all of which are embedded in structures of the same type
* sd     - points to a syfs_dirent structure that represents this kobject in sysfs 
* kref   - provides reference counting

Kobject is used to create kobject directory in /sys.

## SysFS in Linux Device Driver

1. Create a directory in /sys
2. Create Sysfs file

### Create a directory in /sys

We can use this function (kobject_create_and_add) to create a directory
```
struct kobject * kobject_create_and_add (const char * name, struct kobject * parent)
```
Where,
* name   - the name for the kobject
* parent - the parent kobject of this kobject

    * Passing kernel_kobj as parent will create a directory under /sys/kernel/
    * Passing firmware_kobj as parent  will create a directory under /sys/firmware/
    * Passing fs_kobj as parent will create a directory under /sys/fs/
    * Passing NULL as second argument will create a directory under /sys/.

This function creates a kobject structure dynamically and registers it with sysfs. If the kobject was not created
NULL will be returned
When you are finished with this structure, call kobject_put and the structure will be dynamically freed when it is 
is no longer being used.

```
struct kobject *kobj_ref;

/*Creating a directory in /sys/kernel/ */
kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj); //sys/kernel/etx_sysfs

/*Freeing Kobj*/
kobject_put(kobj_ref);
```

### Create Sysfs file

We can create the sysfs files using sysfs attributes. Attributes are represented as regular files in sysfs with one 
value per file. There are lots of helper functions that can be used to create the kobject attributes. They can be found
in the header file sysfs.h.

#### Create attribute
    
Kobj_attribute is defined as.
```
struct kobj_attribute {
 struct attribute attr;
 ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
 ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
};
```
Where,
* attr  - the attribute representing the file to be created
* show  - the pointer to the function that will be called when the file is read in sysfs,
* store - the pointer to the function which will be called when the file is written in sysfs.

We can create an attribute using __ATTR macro.
```
__ATTR(name, permission, show_ptr, store_ptr);
```

#### Create sysfs file

To create a single file attribute we are going to use 'sysfs_create_file'
```
int sysfs_create_file (struct kobject * kobj, const struct attribute * attr);
```
Where,
* kobj   - object we are creating for.
* attr   - attribute descriptor

One can use another function 'sysfs_create_group' to create a group of attributes.
Once you have done using the sysfs file, you should delete the file using the sysfs_remove_file.
```
void sysfs_remove_file(struct kobject * kobj, const struct attribute * attr);
```
Where,
* kobj  - object we're are creating for.
* attr  - attribute descriptor
```
struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d", etx_value);
}

static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count)
{
        sscanf(buf,"%d",&etx_value);
        return count;
}

//This Function will be called from Init function
/*Creating a directory in /sys/kernel/ */
kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);
 
/*Creating sysfs file for etx_value*/
if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
    printk(KERN_INFO"Cannot create sysfs file......\n");
    goto r_sysfs;
}
//This should be called from exit function
kobject_put(kobj_ref); 
sysfs_remove_file(kernel_kobj, &etx_attr.attr);
```
