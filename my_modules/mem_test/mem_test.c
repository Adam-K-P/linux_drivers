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

static void __init mem_clean (void)
{
        printk(KERN_NOTICE "Cleaning up module\n");
        printk(KERN_NOTICE "Module cleaned\n");
}

static int __init mem_init (void)
{
        printk(KERN_NOTICE "Initializing module\n");
        printk(KERN_NOTICE "Module initialized\n");
        return 0;
}

module_init(mem_init);
module_exit(mem_clean);











