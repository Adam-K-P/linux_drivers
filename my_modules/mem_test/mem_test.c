#include "mem_test.h" // Everything 

MODULE_AUTHOR("Adam Pinarbasi");
MODULE_LICENSE("Dual BSD/GPL");

static u64 patterns[] = {
   //default patterns unless otherwise specified
   //17 entries in total
	0,
	0xffffffffffffffffULL,
	0x5555555555555555ULL,
	0xaaaaaaaaaaaaaaaaULL,
	0x1111111111111111ULL,
	0x2222222222222222ULL,
	0x4444444444444444ULL,
	0x8888888888888888ULL,
	0x3333333333333333ULL,
	0x6666666666666666ULL,
	0x9999999999999999ULL,
	0xccccccccccccccccULL,
	0x7777777777777777ULL,
	0xbbbbbbbbbbbbbbbbULL,
	0xddddddddddddddddULL,
	0xeeeeeeeeeeeeeeeeULL,
	0x7a6c7258554e494cULL, /* yeah ;-) */
};

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
   unsigned long addr;
   unsigned long leng;
   struct mem_block *next;
};

struct mem_list {
   int length;
   struct mem_block *head;
   struct mem_block *tail;
};

struct mem_device {
   struct mem_list *mem;
   struct cdev mem_cdev;
   unsigned long size;
   unsigned long nr_tests;
};
struct mem_device mem_dev;

static void clear_list (void) 
{
   struct mem_block *curr = mem_dev.mem->head;

   while (curr != NULL) {
      struct mem_block *temp = curr;
      curr = curr->next;
      kfree(temp);
   }
   kfree(mem_dev.mem);
}

static void add_block (unsigned long addr, unsigned long leng) 
{
   struct mem_block *this_block;

   this_block = kmalloc(sizeof(struct mem_block), GFP_KERNEL);
   memset(this_block, 0, sizeof(struct mem_block));
   this_block->next = NULL;
   this_block->addr = addr;
   this_block->leng = leng;

   if (mem_dev.mem->head == NULL) {
      mem_dev.mem->head = this_block;
      mem_dev.mem->tail = this_block;
      return;
   }

   mem_dev.mem->tail->next = this_block;
   mem_dev.mem->tail = this_block;
}

/*static ssize_t test_mem (struct file *filp) 
{
   return 0;
}*/

int mem_open (struct inode *inode, struct file *filp) 
{
   printk(KERN_NOTICE "Opening file\n");
   return 0;
}

int mem_release (struct inode *inode, struct file *filp)
{
   printk(KERN_NOTICE "Closing file\n");
   clear_list();
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
   char *kern_buf; 
   printk(KERN_NOTICE "Write begin\n");
   kern_buf = kmalloc(count, GFP_KERNEL);
   memset(kern_buf, 0, count);

   if (_copy_from_user(kern_buf, buf, count)) {
      printk(KERN_WARNING "Error reading user input\n");
      return -EFAULT;
   }
   printk(KERN_NOTICE "read: %s from user\n", kern_buf);
   if (kstrtoul(kern_buf, 0, &(mem_dev.nr_tests)) < 0) 
      printk(KERN_WARNING "Improper input from user\n");

   kfree(kern_buf);
   *f_pos += (loff_t)count;
   printk(KERN_NOTICE "read: %lu from user\n", mem_dev.nr_tests);
   return (ssize_t)count;
}

static void mem_test_clean (void)
{
   printk(KERN_NOTICE "Cleaning up module\n");
   cdev_del(&mem_dev.mem_cdev);
   unregister_chrdev(mem_major, "mem_test");
   printk(KERN_NOTICE "module cleaned\n");
}

static void __init reg_cdev (void) 
{
   int err, dev_err = MKDEV(mem_major, mem_minor);
   cdev_init(&mem_dev.mem_cdev, &mem_fops);
   mem_dev.mem_cdev.owner = THIS_MODULE;
   mem_dev.mem_cdev.ops = &mem_fops;
   err = cdev_add(&mem_dev.mem_cdev, dev_err, 1);
   if (err) printk(KERN_WARNING "Error adding mem_dev: %d\n", err);
}

static void __init initialize_mem_list (void) 
{
   mem_dev.mem = kmalloc(sizeof(struct mem_list), GFP_KERNEL);
   memset(mem_dev.mem, 0, sizeof(struct mem_list));

   mem_dev.mem->length = 0;
   mem_dev.mem->head   = NULL;
   mem_dev.mem->tail   = NULL;
}

static int __init mem_test_init (void)
{
   printk(KERN_NOTICE "Module starting\n");
   initialize_mem_list();
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

module_init(mem_test_init);
module_exit(mem_test_clean);











