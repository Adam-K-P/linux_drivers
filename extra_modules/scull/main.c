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

#include "scull.h"		/* local definitions */

MODULE_LICENSE("Dual BSD/GPL");


/* int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET; */

struct file_operations scull_fops = {
   .owner = THIS_MODULE,
};

static void __exit scull_clean(void)
{
   printk(KERN_ALERT "yo\n");

}

static int __init scull_init(void)
{
   return 0;
}

module_init(scull_init);
module_exit(scull_clean);




