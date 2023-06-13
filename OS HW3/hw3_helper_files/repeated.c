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
int string_size = 0;


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
    module_string = NULL;
    if (my_major < 0)
    {
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }

    return 0;
}


void cleanup_module(void)
{
    // This function is called when removing the module using rmmod

    unregister_chrdev(my_major, MY_DEVICE);
    if (module_string != NULL) {
        kfree(module_string);
        module_string = NULL;
    }

    return;
}

/* Initialize values needed for operation if they were not already, default string should be NULL, and max size = 0*/
int my_open(struct inode *inode, struct file *filp)
{
    // handle open
    //unsigned int minor = MINOR(inode->i_rdev);
    unsigned int minor = iminor(inode);
    filp->f_pos = 0;
    return 0;
}


int my_release(struct inode *inode, struct file *filp)
{
    // handle file closing
    // maybe not needed?
    return 0;
}

// Does the read operation. Returns the amount of bytes added to the buffer
ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    // 1 char = 1 byte = 8 bits
    if (buf == NULL) {
        return -EFAULT;
    }

    //bool full = false;
    int bytes_read = 0;
    int allowed_bytes = max_size - *f_pos;

    int leftover = *f_pos
    while(leftover >= string_size) {
        leftover -=string_size;
    }

    int string_pos = leftover;
    for(;bytes_read < allowed_bytes; bytes_read++)
    {
        if(bytes_read == count - 1) { 
            //full = true
            break;
        }
        
        //copy byte to user
        if (copy_to_user(&buf[bytes_read], &module_string[string_pos], 1) != 0) {
            return -EBADF;
        }

        string_pos++;

        if(string_pos >= string_size)
        {
            string_pos = 0;
        }
    }
    
    *f_pos += bytes_read;

    return ++bytes_read;
}

// Ignores writing, but updates max_size to max_size+count, returns count
ssize_t my_write(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    size_t new_size = max_size + count;

    /* char *newstring = kmalloc((new_size + 1) * sizeof(char), GFP_KERNEL);

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

    memcpy(newstring, module_string, max_size);
    newstring[new_size] = '\0';

    kfree(module_string);
    module_string = newstring; */

    max_size = new_size;
    return count; 
}

// Sets llseek to be buffer_pointer+shift. if it over flows you reach 0 or max_size
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

// Our IOCTL procedures.
int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
    case SET_STRING: // fills the string

        // UPDATE STRING SIZE
        // kmalloc is (string_size + 1)
        // also dont forget to do \0 in the end of the string

        char* new_string = (char*)arg;
        if(module_string !=NULL){
            kfree(module_string);
            module_string = NULL;
        }

        int i = 0;
        // is the new string terminated by '\0'?
        while(new_string[i] != '\0') { 
            i++;
        } 
        string_size = i;
        module_string = kmalloc(sizeof(char)*(string_size + 1), GFP_KERNEL);
        if (module_string == NULL) {
            // kmalloc() error
            return -ENOMEM; 
        }

        if (copy_from_user(module_string, new_string, string_size) != 0) {
            // Error copying string from user
            kfree(module_string);
            module_string = NULL;
            return -EBADF; 
        }
        module_string[i] = '\0';
        
	    break;

    case RESET: // returns the buffer to string = null
        max_size = 0;
        string_size = 0;
        if(module_string !=NULL) {
            kfree(module_string);
            module_string = NULL;
        }
	    break;
    
    default:
	    return -ENOTTY;
    }

    return 0;
}
