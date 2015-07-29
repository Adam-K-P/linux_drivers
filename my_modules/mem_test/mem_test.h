#ifndef _MEM_T_H
#define _MEM_T_H

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
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/memblock.h>
#include <linux/smp.h>
#include <linux/pfn.h>
#include <asm/uaccess.h>	/* copy_*_user */

int mem_open (struct inode *, struct file *);
int mem_release (struct inode *, struct file *);
ssize_t mem_read (struct file *, char __user *, size_t, loff_t *);
ssize_t mem_write (struct file *, const char __user *, size_t, loff_t *);


#endif 

