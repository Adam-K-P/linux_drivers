#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);


static int __init hello_init(void)
{
        for(; howmany > 0; --howmany) {
                printk(KERN_ALERT "Hello, %s\n", whom);
                printk(KERN_ALERT "the process is \"%s\" (pid %i)\n", 
                                   current->comm, current->pid);
        }
        return 0;
}

static void hello_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
