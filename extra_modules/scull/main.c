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


/* int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET; */

unsigned int count = 4;
unsigned int firstminor = 0;
dev_t dev = 0;

struct file_operations scull_fops = {
   .owner = THIS_MODULE,
};

static void __exit scull_clean(void)
{
   unregister_chrdev_region(dev, count);
}

static int __init scull_init(void)
{
   int result = alloc_chrdev_region(&dev, firstminor, count, "scull");
   //struct cdev *my_cdev = cdev_alloc();
   return 0;
}

module_init(scull_init);
module_exit(scull_clean);
