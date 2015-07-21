// Adam Pinarbasi
/* This module is designed to allow low-memory condition testing.
   If you wish to use this module, you can allocate some amount of 
   memory by setting the global variables "quantum" and "qset" at
   module-load time.  A qset, by default, is 1000 bits of memory
   and a quantum is, by default, 4000 bits of memory.  The amount
   of memory that will be reserved is equal to (quantum * qset).
   This means that, by default, this program will allocate 4 Mb
   of memory. */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

MODULE_LICENSE("Dual BSD/GPL");

struct mem_device {
        void *data;
        struct mutex mutex;
};
struct mem_device mem_dev;

static void mem_clean (void)
{
        printk(KERN_NOTICE "Cleaning up module\n");
        printk(KERN_NOTICE "Module cleaned\n");
}

static int mem_init (void)
{
        printk(KERN_NOTICE "Initializing module\n");
        printk(KERN_NOTICE "Module initialized\n");
        return 0;
}

module_init(mem_init);
module_exit(mem_clean);











