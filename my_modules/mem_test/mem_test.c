/* Adam Pinarbasi
 * mem_test */

/* TODO
 * No more allocating total system memory.
 * Instead test all the system memory for mem-corruption */
 
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	
#include <linux/slab.h>		
#include <linux/fs.h>		
#include <linux/errno.h>	
#include <linux/types.h>	
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <asm/page.h>

#define OPTIONS (__GFP_HIGH | __GFP_REPEAT | __GFP_HIGHMEM | __GFP_DMA)

MODULE_AUTHOR("Adam Pinarbasi");
MODULE_LICENSE("Dual BSD/GPL"); 

static u64 patterns[] = {
   //default patterns unless otherwise specified
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
	0x7a6c7258554e494cULL,  /* Yeah ;-) */
};

unsigned int mem_minor = 0;
unsigned int mem_major;

/* Function declarations */
int mem_open (struct inode *, struct file *);
int mem_release (struct inode *, struct file *);
ssize_t mem_read (struct file *, char __user *, size_t, loff_t *);
ssize_t mem_write (struct file *, const char __user *, size_t, loff_t *);

struct file_operations mem_fops = {
   owner:   THIS_MODULE,
   open:    mem_open,
   read:    mem_read,
   release: mem_release,
   write:   mem_write, 
};

/*============================================================================*/
/* Data structures used in this module                                        */
/*============================================================================*/

struct mem_block {
   char *identifier;
   struct page *page;
   unsigned int order;
   unsigned long addr;
   unsigned long leng;
   struct mem_block *next;
};

//implements list data structure of mem_blocks
struct mem_list {
   struct mem_block *head;
   struct mem_block *tail;
};

struct mem_device {
   struct mem_list *mem;
   struct cdev mem_cdev;
   unsigned long nr_tests;
   unsigned long stress_amt;
   u64 user_pattern;
};
struct mem_device mem_dev;

/*===========================================================================*/
/* Functions for testing low memory conditions                               */
/*===========================================================================*/

static void add_stress_block (struct page *page, unsigned int order)
{
   struct mem_block *stress_block = kmalloc(sizeof(struct mem_block), GFP_KERNEL);
   memset(stress_block, 0, sizeof(struct mem_block));
   stress_block->page  = page;
   stress_block->order = order;
   stress_block->next  = NULL;
   if (mem_dev.mem->head == NULL) 
      mem_dev.mem->head = stress_block;
   else 
      mem_dev.mem->tail->next = stress_block;
   mem_dev.mem->tail = stress_block;
}

/* Attempts to perform the same allocation with a lower order */
static void handle_fail (unsigned int order) 
{
   unsigned int new_order = order;
   unsigned int nr_allocs = 1;
   struct page *page = NULL;
   unsigned int i;

   while (page == NULL) {
      if (new_order == 0) {
         printk(KERN_WARNING "cannot perform memory allocation\n");
         return;
      }
      new_order -= 1;
      nr_allocs <<= 1;
      printk(KERN_NOTICE "new_order: %d\nnr_allocs: %d\n", new_order, 
                                                           nr_allocs);
      for (i = 0; i < nr_allocs; ++i) {
         page = NULL;
         page = alloc_pages(OPTIONS, new_order);
         if (page == NULL) break;
         add_stress_block(page, new_order);
      }
   }
}

static void amt_specified (void)
{
   unsigned int nr_pages, cnr_pages; //cnr_pages stores original value
   unsigned int track_pgs; //used to track number of pages allocated
   unsigned int order = 0; 
   const unsigned long amount = mem_dev.stress_amt;
   struct page *page = NULL;

   nr_pages = (unsigned int)(amount / (const unsigned long)PAGE_SIZE);
   while (nr_pages != 0) {
      for (track_pgs = 1, cnr_pages = nr_pages, order = 0;;) {
         cnr_pages >>= 1;
         if (cnr_pages == 0) break;
         else {
            ++order;
            track_pgs <<= 1;
         }
      }
      if (order) {
         page = alloc_pages(OPTIONS, order);
         if (page == NULL) 
            handle_fail(order);
         else 
            add_stress_block(page, order);
      }
      nr_pages -= track_pgs;
   }
}

static void amt_unspecified (void) 
{
   struct page *page = NULL;

   //basically puts a lot of process memory into swap files
   while (true) {
      page = alloc_page(OPTIONS);
      if (page == NULL) break; 
      add_stress_block(page, 0);
   }
}

static void stress_test (void) 
{
   current->flags = PF_MEMALLOC; //task is used to allocate memory
   if (mem_dev.stress_amt != 0) 
      amt_specified();
   else 
      amt_unspecified();
}

/*===========================================================================*/
/* Functions for testing memory corruption                                   */
/*===========================================================================*/

/* do_one_pass 
 * mm/memtest.c used as reference for this function*/

static void do_one_pass (u64 pattern) 
{
   u64 *p, *start, *end;
   phys_addr_t start_phys_aligned;
   phys_addr_t start_bad = 0, last_bad = 0;
   const size_t incr = sizeof(pattern);

   start_phys_aligned = ALIGN((phys_addr_t)mem_dev.mem->head->addr, incr);
   start = __va(start_phys_aligned);
   end = start + ((phys_addr_t)mem_dev.mem->head->leng 
                  - (start_phys_aligned 
                      - (phys_addr_t)mem_dev.mem->head->addr)) / incr;

   for (p = start; p < end; ++p) 
      *p = pattern;

   for (p = start; p < end; ++p, start_phys_aligned += incr) {
      if (*p == pattern)
         continue;
      if (start_phys_aligned == last_bad + incr) {
         last_bad += incr;
         continue;
      }
      if (start_bad) //put this in the read sys-call?
         printk(KERN_WARNING "Bad memory found at %llx", *p);
      start_bad = last_bad = start_phys_aligned;
   }
   if (start_bad)
      printk(KERN_WARNING "Bad memory found at %llx", *p);
}

static ssize_t test_mem (void) 
{
   unsigned int i, j;

   printk(KERN_INFO "mem_test: performing %lu tests\n", mem_dev.nr_tests);
   for (i = 0; i < mem_dev.nr_tests; ++i) {
      for (j = 0; j < ARRAY_SIZE(patterns); ++j) 
         do_one_pass(patterns[j]);
   }

   return 0;
}

/*===========================================================================*/
/* Data structure manipulation functions                                     */
/*===========================================================================*/

static void initialize_mem_list (void) 
{
   mem_dev.mem = kmalloc(sizeof(struct mem_list), GFP_KERNEL);
   memset(mem_dev.mem, 0, sizeof(struct mem_list));
   mem_dev.mem->head   = NULL;
   mem_dev.mem->tail   = NULL;
}

static void clear_list (void) 
{
   struct mem_block *curr = mem_dev.mem->head;

   while (curr != NULL) {
      struct mem_block *temp = curr;
      if (temp->page != NULL) 
         __free_pages(temp->page, temp->order);
      curr = curr->next;
      kfree(temp);
   }
   kfree(mem_dev.mem);
}

static void print_blocks (void) 
{
   struct mem_block *curr = mem_dev.mem->head;

   while (curr != NULL) {
      printk(KERN_NOTICE "address: %lu\nlength: %lu\n", curr->addr, 
                                                        curr->leng);
      curr = curr->next;
   }
}

static void add_block (unsigned long addr, unsigned long leng) 
{
   struct mem_block *this_block;

   this_block = kmalloc(sizeof(struct mem_block), GFP_KERNEL);
   memset(this_block, 0, sizeof(struct mem_block));
   this_block->next = NULL;
   this_block->addr = addr;
   this_block->leng = leng;
   this_block->page = NULL;

   if (mem_dev.mem->head == NULL) {
      mem_dev.mem->head = this_block;
      mem_dev.mem->tail = this_block;
      return;
   }

   mem_dev.mem->tail->next = this_block;
   mem_dev.mem->tail = this_block;
}

/*===========================================================================*/
/* Functions for handling user input                                         */
/*===========================================================================*/

static char *extract_comm (char *command) 
{
   int i = 0;
   char *new_comm = kmalloc(strlen(command), GFP_KERNEL);

   for (; i < strlen(command); ++i) {
      char this_c = command[i + 1];
      if ((this_c > '9' || this_c < '0') && this_c != '\0') {
         printk(KERN_WARNING "Improper input from user\n");
         return NULL;
      }
      new_comm[i] = command[i + 1];
   }
   return new_comm;
}

static ssize_t handle_pattern (char *command) 
{
   ssize_t err;
   char *new_comm = extract_comm(command);

   if (new_comm == NULL) {
      printk(KERN_WARNING "Improper input from user\n");
      return -EINVAL;
   }

   err = kstrtou64(new_comm, 0, &(mem_dev.user_pattern));
   if (err < 0) {
      printk(KERN_WARNING "Improper input from user\n");
      return err;
   }
   printk(KERN_NOTICE "%llu\n", mem_dev.user_pattern);
   kfree(new_comm);
   return 0;
}

static ssize_t handle_ul (char *command, unsigned long *dest) 
{
   ssize_t err;
   char *new_comm = extract_comm(command);

   if (new_comm == NULL) {
      printk(KERN_WARNING "Improper input from user\n");
      return -EINVAL;
   }
   err = kstrtoul(new_comm, 0, dest);
   if (err < 0) {
      printk(KERN_WARNING "Improper input from user\n");
      return err;
   }
   printk(KERN_NOTICE "%lu\n", *dest);
   kfree(new_comm);
   return 0;
}

static ssize_t form_block (char *command, size_t addr_l, size_t leng_l) 
{
   int i = 0, j = 0;
   ssize_t err = 0;
   char *addr_s = kmalloc(addr_l, GFP_KERNEL);
   char *leng_s = kmalloc(leng_l, GFP_KERNEL);
   unsigned long addr, leng;

   for (; i < addr_l; ++i) //input correctness already established
      addr_s[i] = command[i + 1];
   addr_s[i - 1] = '\0'; //replace space with plug
   for (; i < strlen(command); ++i, ++j) 
      leng_s[j] = command[i + 1];

   err = kstrtoul(addr_s, 0, &addr);
   if (err < 0) goto err; 
   err = kstrtoul(leng_s, 0, &leng);
   if (err < 0) goto err; 

   add_block(addr, leng);
   kfree(addr_s);
   kfree(leng_s);
   return 0;

err:
   printk(KERN_WARNING "kstrtoul error reading input\n");
   return err;
}

static ssize_t handle_mem (char *command)
{
   int space = 0, i = 0; // space is 0 until a space char has been read 
   ssize_t err = 0;
   size_t leng_l = 0, addr_l = 1; // how much memory to kmalloc 
                          // addr_l '\0' byte preemptively accounted for 
   for (; i < strlen(command); ++i) { //just get lengths here
      char this_c = command[i + 1];
      if (this_c == ' ' && space == 0) space = 1;
      else if (space == 0) ++addr_l;
      else ++leng_l;
   }
   if (space == 0) goto err;
   err = form_block(command, addr_l, leng_l);
   if (err < 0) goto err;
   return 0;

err:
   printk(KERN_NOTICE "error in function 'handle_mem'\n");
   return err;
}

static ssize_t perf_comm (char *command)
{
   ssize_t err = 0;

   if      (*command == 'p') err = handle_pattern(command);
   else if (*command == 'n') err = handle_ul(command, &(mem_dev.nr_tests));
   else if (*command == 'm') err = handle_mem(command);
   else if (*command == 'c') err = test_mem();
   else if (*command == 's') { 
      err = handle_ul(command, &(mem_dev.stress_amt));
      stress_test();
   }

   if (err < 0) return err;
   return 0;
}

/*===========================================================================*/
/* Char-device functions                                                     */
/*===========================================================================*/

int mem_open (struct inode *inode, struct file *filp) 
{
   printk(KERN_NOTICE "Opening file\n");
   initialize_mem_list();
   return 0;
}

int mem_release (struct inode *inode, struct file *filp)
{
   current->flags = PF_LESS_THROTTLE; //for cleaning memory
   printk(KERN_NOTICE "Closing file\n");
   print_blocks();
   clear_list();
   return 0;
}

/* mem_read 
 *
 * read system call is used to send information back to client 
 * more specifically, it will return the identifier of the last
 * mem_block added */

ssize_t mem_read (struct file *filp, char __user *buf, size_t count,
                  loff_t *f_pos)
{
   printk(KERN_NOTICE "Read begin\n");
   return 0;
}

/* mem_write
 *
 * write system call is used to take in commands from the client */
ssize_t mem_write (struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos) 
{
   char *command; 
   long ret;
   ssize_t err;

   printk(KERN_NOTICE "Write begin\n");
   command = kmalloc(count, GFP_KERNEL);
   memset((void *)command, 0, count);

   ret = strncpy_from_user(command, buf, (long)count);
   if (ret < 0) {
      printk(KERN_WARNING "Error reading user input\n");
      return -EFAULT;
   }
   printk(KERN_NOTICE "read: %s from user\n", command);

   err = perf_comm(command);
   if (err < 0) 
      return err;

   kfree(command);
   *f_pos += (loff_t)ret;
   if (*f_pos >= (loff_t)count) 
      return 0;
   return (ssize_t)ret;
}

/*===========================================================================*/
/* Functions for loading/unloading module                                    */
/*===========================================================================*/

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

static int __init mem_test_init (void)
{
   printk(KERN_NOTICE "Module starting\n");
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











