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
#include <asm/uaccess.h>	/* copy_*_user */

int mem_open (struct inode *inode, struct file *filp);
int mem_release (struct inode *inode, struct file *filp);
ssize_t mem_read (struct file *file, char __user *buf, size_t count,
                  loff_t *f_pos);
ssize_t mem_write (struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos);

#endif 

