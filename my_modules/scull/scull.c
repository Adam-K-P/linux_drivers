// Adam Pinarbasi
/* This module is a Simple Character Utility for Loading Localities (scull).
   An accompanying program, test, will transfer anything you write
   to the character device scull in kernel space (using the write system 
   call) then echo back what you wrote into user space (using the read
   system call). */

#include "scull.h"		/* local definitions */

MODULE_LICENSE("Dual BSD/GPL");

unsigned int scull_minor = 0;
unsigned int scull_major;

struct scull_qset {
   void *data;
};

struct scull_device {
   struct scull_qset *qset;
   struct cdev cdev;
   unsigned long size;
   struct mutex mutex;
};
struct scull_device sdev;

/* Declaration of scull functions */

struct file_operations scull_fops = {
   owner:    THIS_MODULE,
   open:     scull_open,
   read:     scull_read,
   release:  scull_release,
   write:    scull_write,
};

int scull_open(struct inode *inode, struct file *filp)
{
   printk(KERN_WARNING "Opening file\n");
   if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
      if (mutex_lock_interruptible(&sdev.mutex))
         return -ERESTARTSYS;
      mutex_unlock(&sdev.mutex);
   }
   return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
   /*if (mutex_lock_interruptible(&sdev.mutex))
      return -ERESTARTSYS;*/
   kfree(sdev.qset->data);
   //mutex_unlock(&sdev.mutex);
   printk(KERN_WARNING "Closing file\n");
   return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos)
{
   int copy_res;
   printk(KERN_WARNING "Read begin\n");
   if (sdev.qset->data == NULL) {
      printk(KERN_WARNING "Nothing here to read\n");
      return 0;
   }
   /*if (mutex_lock_interruptible(&sdev.mutex))
      return -ERESTARTSYS;*/
   copy_res = _copy_to_user(buf, sdev.qset->data, count);
   //mutex_unlock(&sdev.mutex);
   if (copy_res) {
      printk(KERN_WARNING "scull_read: Failed to transfer %d bytes \n", 
             copy_res);
      return -EFAULT;
   }
   *f_pos += count;
   printk(KERN_WARNING "Read end\n");
   return count;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
{
   int res;
   printk(KERN_WARNING "Write begin\n");
   /*if (mutex_lock_interruptible(&sdev.mutex))
      return -ERESTARTSYS;*/
   sdev.qset->data = kmalloc(count, GFP_KERNEL);
   if (sdev.qset->data == NULL) {
      printk(KERN_WARNING "Memory unable to be initialized\n");
      return -EFAULT;
   }
   memset(sdev.qset->data, 0, count);
   res = _copy_from_user(sdev.qset->data, buf, count);
   //mutex_unlock(&sdev.mutex);
   if (res) {
      printk(KERN_WARNING "scull_write: Failed to transfer %d bytes\n", res);
      return -EFAULT;
   }
   printk(KERN_WARNING "Read string: %s from user\n", (char *)sdev.qset->data);
   *f_pos += (loff_t) count;
   printk(KERN_WARNING "Write end\n");
   return (ssize_t)count;
}

static void scull_clean(void)
{
   printk(KERN_WARNING "Cleaning up module\n");
   cdev_del(&sdev.cdev);
   unregister_chrdev(scull_major, "scull");
   printk(KERN_WARNING "Module cleaned\n");
}

static void reg_cdev(void) 
{
   int err, dev_err = MKDEV(scull_major, scull_minor);
   cdev_init(&sdev.cdev, &scull_fops);
   sdev.cdev.owner = THIS_MODULE;
   sdev.cdev.ops   = &scull_fops;
   err = cdev_add(&sdev.cdev, dev_err, 1);
   if (err) printk(KERN_WARNING "Error adding scull: %d\n", err);
}

static int scull_init(void)
{
   int result;
   struct scull_qset *this_qset;
   printk(KERN_WARNING "Initializing module\n");
   this_qset = kmalloc(sizeof(struct scull_qset), GFP_KERNEL); 
   sdev.qset = this_qset;
   scull_major = register_chrdev(0, "scull", &scull_fops );
   if (scull_major < 0) {
      printk(KERN_WARNING "scull: can't get major\n"); 
      return result;
   }
   reg_cdev();
   printk(KERN_WARNING "Module initialized\n");_
   printk(KERN_ALERT "major number is %d\n", scull_major);
   return 0;
}

module_init(scull_init);
module_exit(scull_clean);
