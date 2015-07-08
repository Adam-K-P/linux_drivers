// Adam Pinarbasi
/* This module is designed to allow low-memory condition testing.
   If you wish to use this module, you can allocate some amount of 
   memory by setting the global variables "quantum" and "qset" at
   module-load time.  A qset, by default, is 1000 bits of memory
   and a quantum is, by default, 4000 bits of memory.  The amount
   of memory that will be reserved is equal to (quantum * qset).
   This means that, by default, this program will allocate 4 Mb
   of memory. */

   
#include "mem_test.h"   /* local definitions */

MODULE_LICENSE("Dual BSD/GPL");

unsigned int mem_minor = 0;
unsigned int mem_major;
long quantum = 4000;
long qset    = 1000;

struct mem_device {
   void *data;
   struct cdev cdev;
   struct mutex mutex;
};
struct mem_device mem_dev;

struct file_operations mem_fops = {
   owner:   THIS_MODULE,
   open:             mem_open,
   read:             mem_read,
   release:          mem_release,
   write:            mem_write,
   unlocked_ioctl:   mem_ioctl,
};

long mem_ioctl (struct inode *inode, struct file *filp, unsigned int cmd,
                                                        unsigned long arg) { 
   return 0;
}

int mem_open (struct inode *inode, struct file *filp) 
{
   printk(KERN_WARNING "Opening file\n");
   return 0;
}

int mem_release (struct inode *inode, struct file *filp) 
{
   printk(KERN_WARNING "Closing file\n");
   return 0;
}

ssize_t mem_read (struct file *filp, char __user *buf, size_t count,
                    loff_t *f_pos) 
{
   printk(KERN_WARNING "Read begin\n");
   printk(KERN_WARNING "Read end\n");
   return 0;
}

ssize_t mem_write (struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos)
{
   printk(KERN_WARNING "Write begin\n");
   printk(KERN_WARNING "Write end\n");
   return 0;
}

static void mem_clean (void)
{
   printk(KERN_WARNING "Cleaning up module\n");
   cdev_del(&mem_dev.cdev);
   unregister_chrdev(mem_major, "mem_test");
   printk(KERN_WARNING "Module cleaned\n");
}

static void reg_cdev (void) 
{
   int err, dev_err = MKDEV(mem_major, mem_minor);
   cdev_init(&mem_dev.cdev, &mem_fops);
   mem_dev.cdev.owner = THIS_MODULE;
   mem_dev.cdev.ops   = &mem_fops;
   err = cdev_add(&mem_dev.cdev, dev_err, 1);
   if (err) printk(KERN_WARNING "Error adding mem_test: %d\n", err);
}

static int mem_init (void)
{
   int result;
   printk(KERN_WARNING "Initializing module\n");
   mem_major = register_chrdev(0, "mem_test", &mem_fops);
   if (mem_major < 0) {
      printk(KERN_WARNING "mem_test: can't get major\n");
      return result;
   }
   reg_cdev();
   printk(KERN_WARNING "Module initialized\n");
   printk(KERN_ALERT "Major number is %d\n", mem_major);
   return 0;
}

module_init(mem_init);
module_exit(mem_clean);











