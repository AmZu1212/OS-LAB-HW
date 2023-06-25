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

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */
char *module_string = "hello";//NULL;
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
    printk("Started init_module\n");
    // This function is called when inserting the module using insmod
    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);
    module_string = NULL;
    if (my_major < 0)
    {
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }
    printk("Finished init_module\n");
    return 0;
}

void cleanup_module(void)
{
    // This function is called when removing the module using rmmod
    printk("Started cleanup_module\n");
    unregister_chrdev(my_major, MY_DEVICE);
    if (module_string != NULL)
    {
        kfree(module_string);
        module_string = NULL;
    }
    printk("Finished cleanup_module\n");
    return;
}

/* Initialize values needed for operation if they were not already, default string should be NULL, and max size = 0*/
int my_open(struct inode *inode, struct file *filp)
{
    // handle open
    // unsigned int minor = MINOR(inode->i_rdev);
    // unsigned int minor = iminor(inode); // IS THIS USELESS?
    // access_ok();
    printk("called open()\n");
    filp->f_pos = 0;
    return 0;
}

int my_release(struct inode *inode, struct file *filp)
{
    // handle file closing
    // maybe not needed?
    printk("called release\n");
    return 0;
}

// Does the read operation. Returns the amount of bytes added to the buffer
ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    
    // 1 char = 1 byte = 8 bits
    if (buf == NULL)
    {
        return -EFAULT;
    }

    if (access_ok(VERIFY_READ, buf, count) == 0)
    {
        return -EBADF;
    }

    printk("called my_read(), with count = %d, and buf = %s \n", count, buf); 
    // bool full = false;
    int bytes_read = 0;
    int allowed_bytes = max_size - *f_pos;
    printk("the allowed number of bytes is %d\n", allowed_bytes);

    int leftover = *f_pos;
    printk("before while loop\n");
    while (leftover >= string_size)
    {
        leftover -= string_size;
    }
    printk("after while loop\n");
    int string_pos = leftover;
    printk("entering for loop\n");
    for (; bytes_read < allowed_bytes && bytes_read < count; bytes_read++)
    {
        if (bytes_read == count - 1)
        {
            printk("done reading bytes (reading last byte)\n");
            // full = true
            // break;
        }

        // copy byte to user
        if (copy_to_user(&buf[bytes_read], &module_string[string_pos], 1) != 0)
        {
            printk("copy to user failed\n");
            return -EBADF;
        }

        string_pos++;

        if (string_pos >= string_size)
        {
            printk("string pos was reset\n");
            string_pos = 0;
	}
    }
    printk("ended for loop, bytes read is: %d\n",bytes_read);
    *f_pos += bytes_read;
    return bytes_read;
}

// Ignores writing, but updates max_size to max_size+count, returns count
ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    size_t new_size = max_size + count;
    max_size = new_size;
    printk("my_write was called, new max_size is: %d\n", max_size);
    return count;
}

// Sets llseek to be buffer_pointer+shift. if it over flows you reach 0 or max_size
loff_t my_llseek(struct file *flip, loff_t shift, int mode)
{
    printk("my_llseek was called\n");
    // 0 <-> max_size
    loff_t newpos = flip->f_pos + shift;

    if (newpos >= max_size)
    {
        newpos = max_size;
    }

    if (newpos <= 0)
    {
        newpos = 0;
    }

    flip->f_pos = newpos;
    return newpos;
}

// Our IOCTL procedures.
int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    
    char *new_string = (char *)arg;
    //char print_string;
    //copy_form_user(print_string, (char *)arg, );


    switch (cmd)
    {
    case SET_STRING: // fills the string
        printk("ioctl called, with SET_STRING\n");
        // UPDATE STRING SIZE
        // kmalloc is (string_size + 1)
        // also dont forget to do \0 in the end of the string
        if(new_string == NULL)
        {
            return -EINVAL;
        }

        printk("trying to set string, given string is: %s\n", new_string);
        if (module_string != NULL)
        {
            kfree(module_string);
            module_string = NULL;
        }

        int i = 0;
        // is the new string terminated by '\0'?
        while (new_string[i] != '\0')
        {
            i++;
        }
        string_size = i;
        printk("new string's size is %d, supposed to be 3 \n", string_size);

        module_string = kmalloc(sizeof(char) * (string_size + 1), GFP_KERNEL);
        if (module_string == NULL)
        {
            // kmalloc() error
            return -ENOMEM;
        }

        if (copy_from_user(module_string, new_string, string_size) != 0)
        {
            // Error copying string from user
            kfree(module_string);
            module_string = NULL;
            return -EBADF;
        }
        module_string[i] = '\0';
	printk("the string we set is : %s\n", module_string);
        break;

    case RESET: // returns the buffer to string = null
        printk("ioctl called, with RESET\n");
        max_size = 0;
        string_size = 0;
        if (module_string != NULL)
        {
            kfree(module_string);
            module_string = NULL;
        }
        break;

    default:
        return -ENOTTY;
    }

    return 0;
}
