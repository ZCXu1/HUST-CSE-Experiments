#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>


static char *name = "xuzichuan";
static int times = 1;

module_param(times,int,0644);
module_param(name, charp,0644);

static int hello_init(void)
{
    int i;
    for(i = 0 ; i < times; i++)
        printk(KERN_ALERT "(%d) hello, %s!\n", i, name);
    return 0;
}
static void hello_exit(void)
{
    printk(KERN_ALERT"Goodbye, %s!\n",name);
}
MODULE_LICENSE("Dual BSD/GPL");
module_init(hello_init);
module_exit(hello_exit);