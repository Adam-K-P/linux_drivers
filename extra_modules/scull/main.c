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

/*These values can be changed at compile time */
#define QUANTUM 4000
#define QSET 1000

unsigned int scull_minor = 0;
unsigned int scull_major;

struct scull_dev {
   struct scull_qset *data;
   struct cdev cdev;
   unsigned int quantum;
   unsigned int qset;
   unsigned long size;
};

struct scull_qset {
   void **data;
   struct scull_qset *next;
};

int scull_open(struct inode *inode, struct file *filp)
{
   struct scull_dev *dev;
   dev = container_of(inode->i_cdev, struct scull_dev, cdev);
   filp->private_data = dev;
   return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
   return 0;
}

/*ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos)
{
}*/


struct file_operations scull_fops = {
   .owner   = THIS_MODULE,
   .open    = scull_open,
   .release = scull_release,
};

static void __exit scull_clean(void)
{
   dev_t dev = MKDEV(scull_major, scull_minor);
   unregister_chrdev_region(dev, 1);
}

static void make_cdev(struct scull_dev *sdev) 
{
   int err;
   int dev_err = MKDEV(scull_major, scull_minor);
   cdev_init(&sdev->cdev, &scull_fops);
   sdev->cdev.owner = THIS_MODULE;
   sdev->cdev.ops   = &scull_fops;
   err = cdev_add(&sdev->cdev, dev_err, 1);
   if (err) 
      printk(KERN_WARNING "Error adding scull: %d\n", err);
}

static int __init scull_init(void)
{
   struct scull_dev sdev;
   dev_t dev = 0;
   int result;
   result = alloc_chrdev_region(&dev, 0, 1, "scull");
   if (result < 0) {
      printk(KERN_WARNING "scull: can't get major\n"); 
      return result;
   }
   make_cdev(&sdev);
   return 0;
}

module_init(scull_init);
module_exit(scull_clean);
