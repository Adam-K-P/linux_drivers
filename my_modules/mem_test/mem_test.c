#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include "mem_test.h"

MODULE_LICENSE("Dual BSD/GPL");

static u64 patterns[] __initdata = {
        /* The first entry has to be 0 to leave memtest with zeroed memory */
	0,
	0x1111111111111111ULL,
	0x2222222222222222ULL,
	0x3333333333333333ULL,
	0x4444444444444444ULL,
	0x5555555555555555ULL,
	0x6666666666666666ULL,
	0x7777777777777777ULL,
	0x8888888888888888ULL,
	0x9999999999999999ULL,
	0xaaaaaaaaaaaaaaaaULL,
	0xbbbbbbbbbbbbbbbbULL,
	0xccccccccccccccccULL,
	0xddddddddddddddddULL,
	0xeeeeeeeeeeeeeeeeULL,
	0xffffffffffffffffULL,
	0x7a6c7258554e494cULL, /* yeah ;-) */
};

struct file_operations mem_fops = {
        owner:   THIS_MODULE,
        open:    mem_open,
        read:    mem_read,
        release: mem_release,
        write:   mem_write, 
};

int mem_open (struct inode *inode, struct file *filp) 
{
        printk(KERN_NOTICE "Opening file\n");
        return 0;
}

int mem_release (struct inode *inode, struct file *filp)
{
        printk(KERN_NOTICE "Closing file\n");
        return 0;
}

ssize_t mem_read (struct file *filp, char __user *buf, size_t count,
                  loff_t *f_pos)
{
        printk(KERN_NOTICE "Read begin\n");
        return 0;
}

ssize_t mem_write (struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos) 
{
        printk(KERN_NOTICE "Write begin\n");
        return 0;
}

static void mem_clean (void)
{
        printk(KERN_NOTICE "Cleaning up module\n");
}

static int __init mem_init (void)
{
        printk(KERN_NOTICE "Module starting\n");
        return 0;
}

module_init(mem_init);
module_exit(mem_clean);











