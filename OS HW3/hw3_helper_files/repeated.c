/* repeated.c: Example char device module.
 *
 */
/* Kernel Programming */
#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/kernel.h>  	
#include <linux/module.h>
#include <linux/fs.h>       		
#include <asm/uaccess.h>
#include <linux/errno.h>  
#include <asm/segment.h>
#include <asm/current.h>

#include "repeated.h"

#define MY_DEVICE "repeated"

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */
char *module_string = NULL;
int max_size = 0;


struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .llseek = my_llseek,
    .ioctl = my_ioctl,
};

int init_module(void)
{
    // This function is called when inserting the module using insmod

    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);

    if (my_major < 0)
    {
	printk(KERN_WARNING "can't get dynamic major\n");
	return my_major;
    }

    //
    // do_init();
    //
    return 0;
}


void cleanup_module(void)
{
    // This function is called when removing the module using rmmod

    unregister_chrdev(my_major, MY_DEVICE);

    //
    // do clean_up();
    //
    return;
}

/* initialize values needed for operation if they were not already, default string should be NULL, and max size = 0*/
int my_open(struct inode *inode, struct file *filp)
{
    // handle open

    return 0;
}


int my_release(struct inode *inode, struct file *filp)
{
    // handle file closing

    return 0;
}

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    //
    // Do read operation.
    // Return the amount of bytes added to the buffer

    return 0; 
}

ssize_t my_write(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    size_t new_size = max_size + count;
    char *newstring = kmalloc((new_size + 1) * sizeof(char), GFP_KERNEL);

    //maybe check for malloc error
    if(newstring == NULL) {
        //kmalloc() error
        return -ENOMEM;
    }
    
    int i = 0;
    while(module_string[i] != NULL) {

        char temp = module_string[i];
        newstring[i] = temp;
        i++;
    }

    newstring[new_size] = '\0';

    free(module_string);

    module_string = newstring;
    max_size = new_size;

    return count; 
}

loff_t my_llseek(struct file *flip, loff_t buffer_pointer, int shift) {
    
    // 0 <-> max_size
    size_t newpos = buffer_pointer + shift;

    if(newpos >= max_size) {
        newpos = max_size;
    }

    if(newpos <= 0) {
        newpos = 0;
    }

    buffer_pointer = newpos;
    return newpos;
}


int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
    case SET_STRING:// fills the string
	//
	// handle 
	//
	break;
    case RESET: // returns the buffer to string = null
	//
	// handle 
	//
	break;
    default:
	return -ENOTTY;
    }

    return 0;
}
