#include "mem_test.h" // Everything 

MODULE_AUTHOR("Adam Pinarbasi");
MODULE_LICENSE("Dual BSD/GPL");

unsigned int mem_minor = 0;
unsigned int mem_major;

struct file_operations mem_fops = {
   owner:   THIS_MODULE,
   open:    mem_open,
   read:    mem_read,
   release: mem_release,
   write:   mem_write, 
};

struct mem_block {
   void *data;
   struct mem_block *next;
};

struct mem_device {
   struct mem_block *mem;
   struct cdev mem_cdev;
   unsigned long size;
   struct mutex memtex;
};
struct mem_device mem_dev;

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
   cdev_del(&mem_dev.mem_cdev);
   unregister_chrdev(mem_major, "mem_test");
   printk(KERN_NOTICE "module cleaned\n");
}

static void reg_cdev (void) 
{
   int err, dev_err = MKDEV(mem_major, mem_minor);
   cdev_init(&mem_dev.mem_cdev, &mem_fops);
   mem_dev.mem_cdev.owner = THIS_MODULE;
   mem_dev.mem_cdev.ops = &mem_fops;
   err = cdev_add(&mem_dev.mem_cdev, dev_err, 1);
   if (err) printk(KERN_WARNING "Error adding mem_dev: %d\n", err);
}


static int __init mem_init (void)
{
   printk(KERN_NOTICE "Module starting\n");
   mutex_init(&mem_dev.memtex);
   mem_dev.mem = kmalloc(sizeof(struct mem_block), GFP_KERNEL);
   mem_major = register_chrdev(0, "mem_test", &mem_fops);
   if (mem_major < 0) {
      printk(KERN_WARNING "mem_test: can't get major number\n");
      return -1;
   }
   reg_cdev();
   printk(KERN_NOTICE "mem_test: module initialized\n");
   printk(KERN_NOTICE "mem_test: major number is %d\n", mem_major);
   return 0;
}

module_init(mem_init);
module_exit(mem_clean);











