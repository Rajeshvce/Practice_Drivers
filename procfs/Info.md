# PROCFS in Linux
	 Procfs is a mount point for the procfs (Process Filesystem) which is a filesystem in memory.
Many processes store information about themselves on this virtual filesystem. ProcFS also stores
other system information.

## Creating PROCFS directory
```
struct proc_dir_entry *proc_mkdir(const char* name, struct proc_dir_entry *parent)
```
Where,
	* name   : the name of the directory created under /proc
	* parent : In case the folder needs to be created in subfolder under /proc a pointer to the same is passed else it can be left as NULL

## Creating Procfs Entry
```
struct proc_dir_entry *proc_create (const char* name, umode_t mode, struct proc_dir_entry *parent, const struct proc_fops *proc_fops)
```

where,
	* name      : The name of the proc entry
	* mode      : The access mode for proc entry
	* parent    : The name of the parent directory under /proc. If NULL is passed as a parent, the /proc directory will be set as parent
	* proc_fops : The structure in which the file operations for the proc entry will be created 

Example:
```
proc_create("etx_proc",0666,NULL,&proc_fops);
```

* create_proc_read_entry 
	* It is a functions specific to create only read entries
* create_proc_entry 
	* It is a generic function that allows creating both the read as well as the write entries

* proc_fops file operations structure example
```
static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};
```

## Remove Proc entry 
```
void remove_proc_entry(const char *name, struct proc_dir_entry *parent);
```
Example:
```
remove_proc_entry("etx_proc",NULL);
```
* Remove the complete parent durectory using the proc_remove(struct proc_dir_entry *parent)


