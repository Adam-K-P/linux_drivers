#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/uaccess.h>	/* copy_*_user */

//#include "scull.h"		/* local definitions */

MODULE_LICENSE("Dual BSD/GPL");

unsigned int count = 1;
dev_t dev = 0;
struct cdev *my_cdev;

int scull_open(struct inode *inode, struct file *filp)
{
   return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
   return 0;
}

struct file_operations scull_fops = {
   .owner   = THIS_MODULE,
   .open    = scull_open,
   .release = scull_release,
};

static void __exit scull_clean(void)
{
   unregister_chrdev_region(dev, count);
   cdev_del(my_cdev);
}

static int make_cdev(void) 
{
   int err;
   my_cdev = cdev_alloc();
   cdev_init(my_cdev, &scull_fops);
   my_cdev->owner = THIS_MODULE;
   my_cdev->ops   = &scull_fops;
   err = cdev_add(my_cdev, dev, 1);
   return err;
}

static int __init scull_init(void)
{
   int result, scull_major, err;
   result = alloc_chrdev_region(&dev, firstminor, count, "scull");
   scull_major = MAJOR(dev);
   printk(KERN_ALERT "Major number is: %d\n", scull_major);
   if (result < 0) {
      printk(KERN_WARNING "scull: can't get major\n"); 
      return result;
   }
   err = make_cdev();
   if (err) {
      printk(KERN_WARNING "scull: can't add char device\n");
      return err;
   }
   return 0;
}

module_init(scull_init);
module_exit(scull_clean);
