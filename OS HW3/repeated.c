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
#include <linux/slab.h>

#include "repeated.h"

#define MY_DEVICE "repeated"
#define MAX_MINORS 256
#define DBG 0

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */

typedef struct moduledata
{
    int flag;
    char *module_string;
    int max_size;
    int string_size;
} Data_t;

Data_t OurData[MAX_MINORS];

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .llseek = my_llseek,
    .ioctl = my_ioctl,
};

// This function is called when inserting the module using insmod
int init_module(void)
{
    if(DBG) printk("Started init_module\n");
    int i = 0;
    for (i = 0; i < MAX_MINORS; i++)
    {
        OurData[i].flag = 0;
        OurData[i].max_size = 0;
        OurData[i].module_string = NULL;
        OurData[i].string_size = 0;
    }

    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);
    if (my_major < 0)
    {
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }
    if(DBG) printk("Finished init_module\n");
    return 0;
}

// This function is called when removing the module using rmmod
void cleanup_module(void)
{
    if(DBG) printk("Started cleanup_module\n");
    int i = 0;
    for (i = 0; i < MAX_MINORS; i++)
    {
        if (OurData[i].module_string != NULL)
        {
            kfree(OurData[i].module_string);
        }
    }
    unregister_chrdev(my_major, MY_DEVICE);
    if(DBG) printk("Finished cleanup_module\n");
    return;
}

/* Initialize values needed for operation if they were not already, default string should be NULL, and max size = 0*/
int my_open(struct inode *inode, struct file *filp)
{
    // handle open
    unsigned int my_minor = MINOR(inode->i_rdev);
    if(DBG) printk("our minor is %d\n", my_minor);
    if (OurData[my_minor].flag != 1)
    {
        OurData[my_minor].flag = 1;
    }

    // save current minor number, in the filp private_data field
    unsigned int *minor_ptr = kmalloc(sizeof(unsigned int), GFP_KERNEL);
    if (minor_ptr == NULL)
    {
        return -ENOMEM;
    }
    *minor_ptr = my_minor;
    filp->private_data = minor_ptr;

    if(DBG) printk("called open()\n");
    filp->f_pos = 0;
    return 0;
}

int my_release(struct inode *inode, struct file *filp)
{
    if(filp->private_data != NULL){
        kfree(filp->private_data);
    }
    // MAYBE FREE PRIVATE DATA
    if(DBG) printk("called release\n");
    return 0;
}

// Does the read operation. Returns the amount of bytes added to the buffer
ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    unsigned int curr_minor = *(unsigned int *)filp->private_data;
    char *module_string = OurData[curr_minor].module_string;
    int max_size = OurData[curr_minor].max_size;
    int string_size = OurData[curr_minor].string_size;

    // 1 char = 1 byte = 8 bits
    if (buf == NULL)
    {
        return -EFAULT;
    }

    if (access_ok(VERIFY_READ, buf, count) == 0)
    {
        return -EBADF;
    }

    if(DBG) printk("called my_read(), with count = %d, and buf = %s \n", count, buf);
    // bool full = false;
    int bytes_read = 0;
    int allowed_bytes = max_size - *f_pos;
    if(DBG) printk("the allowed number of bytes is %d\n", allowed_bytes);

    int leftover = *f_pos;
    if(DBG) printk("before while loop\n");
    while (leftover >= string_size)
    {
        leftover -= string_size;
    }
    if(DBG) printk("after while loop\n");
    int string_pos = leftover;
    if(DBG) printk("entering for loop\n");
    for (; bytes_read < allowed_bytes && bytes_read < count; bytes_read++)
    {
        if (bytes_read == count - 1)
        {
            if(DBG) printk("done reading bytes (reading last byte)\n");
            // full = true
            // break;
        }

        // copy byte to user
        /* if(module_string == NULL){
            printk("string bug????\n");
        } else {
            printk("trying to send %s to user, with minor %d\n", module_string, curr_minor);
        } */

        if (copy_to_user(&buf[bytes_read], &module_string[string_pos], 1) != 0)
        {
            if(DBG) printk("copy to user failed\n");
            return -EBADF;
        }

        string_pos++;

        if (string_pos >= string_size)
        {
            if(DBG) printk("string pos was reset\n");
            string_pos = 0;
        }
    }
    if(DBG) printk("ended for loop, bytes read is: %d\n", bytes_read);
    *f_pos += bytes_read;
    return bytes_read;
}

// Ignores writing, but updates max_size to max_size+count, returns count
ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    unsigned int curr_minor = *(unsigned int *)filp->private_data;
    size_t new_size = OurData[curr_minor].max_size + count;
    OurData[curr_minor].max_size = new_size;
    if(DBG) printk("my_write was called, new max_size is: %d\n", OurData[curr_minor].max_size);
    return count;
}

// Sets llseek to be buffer_pointer+shift. if it over flows you reach 0 or max_size
loff_t my_llseek(struct file *filp, loff_t shift, int mode)
{
    unsigned int curr_minor = *(unsigned int *)filp->private_data;

    if(DBG) printk("my_llseek was called\n");
    // 0 <-> max_size
    loff_t newpos = filp->f_pos + shift;

    if (newpos >= OurData[curr_minor].max_size)
    {
        newpos = OurData[curr_minor].max_size;
    }

    if (newpos <= 0)
    {
        newpos = 0;
    }

    filp->f_pos = newpos;
    return newpos;
}

// Our IOCTL procedures.
int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    unsigned int curr_minor = *(unsigned int *)filp->private_data;

    char *new_string = (char *)arg;

    switch (cmd)
    {
    case SET_STRING: // fills the string
        if(DBG) printk("ioctl called, with SET_STRING\n");
        // UPDATE STRING SIZE
        // kmalloc is (string_size + 1)
        // also dont forget to do \0 in the end of the string
        if (new_string == NULL)
        {
            return -EINVAL;
        }

        if(DBG) printk("trying to set string, given string is: %s\n", new_string);
        if (OurData[curr_minor].module_string != NULL)
        {
            kfree(OurData[curr_minor].module_string);
            OurData[curr_minor].module_string = NULL;
        }

        int i = 0;
        int string_size = 0;
        // is the new string terminated by '\0'?
        while (new_string[i] != '\0')
        {
            i++;
        }
        string_size = i;
        if(DBG) printk("new string's size is %d, supposed to be 3\n", string_size);

        OurData[curr_minor].module_string = kmalloc(sizeof(char) * (string_size + 1), GFP_KERNEL);
        if (OurData[curr_minor].module_string == NULL)
        {
            OurData[curr_minor].string_size = 0;
            // kmalloc() error
            return -ENOMEM;
        }

        if (copy_from_user(OurData[curr_minor].module_string, new_string, string_size) != 0)
        {
            // Error copying string from user
            kfree(OurData[curr_minor].module_string);
            OurData[curr_minor].module_string = NULL;
            OurData[curr_minor].string_size = 0;
            return -EBADF;
        }
        OurData[curr_minor].module_string[i] = '\0';
        OurData[curr_minor].string_size = string_size;
        if(DBG) printk("the string we set is : %s\n", OurData[curr_minor].module_string);

        break;

    case RESET: // returns the buffer to string = null
        if(DBG) printk("ioctl called, with RESET\n");
        OurData[curr_minor].max_size = 0;
        OurData[curr_minor].string_size = 0;
        if (OurData[curr_minor].module_string != NULL)
        {
            kfree(OurData[curr_minor].module_string);
            OurData[curr_minor].module_string = NULL;
        }
        break;

    default:
        return -ENOTTY;
    }

    return 0;
}
