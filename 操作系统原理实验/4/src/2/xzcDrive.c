#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/uaccess.h>

static struct cdev drv;
static dev_t ndev;
static char xzc[32];

static int drv_open(struct inode *nd, struct file *fp) 
{
    int major;
    int minor;
    major = MAJOR(nd->i_rdev);
    minor = MINOR(nd->i_rdev);
    printk(KERN_EMERG "hello open, major = %d, minor = %d\n", major, minor);
    return 0;
}

static ssize_t drv_read(struct file* fp, char __user* u, size_t sz, loff_t* loff)
{
    char res[32];
    int i = 0;
    int num1 = 0;
    int num2 = 0;
    bool flag = true;
    printk(KERN_EMERG "hello read.\n");
    while (xzc[i] != '+'&& xzc[i] != '-')
    {
        num1 = 10 * num1 + (xzc[i] - '0');
        i++;
    }
    if (xzc[i] == '-') flag = false;
    i++;
    while (xzc[i] != '\0') 
    {
        num2 = 10 * num2 + (xzc[i] - '0');
        i++;
    }
    if (flag == true) 
    {
        printk(KERN_EMERG "%d + %d = %d\n", num1, num2, num1 + num2);
        sprintf(res, "%d + %d = %d\n", num1, num2, num1 + num2);
        copy_to_user(u, res, strlen(res));
    }
    else
    {
        printk(KERN_EMERG "%d - %d = %d\n", num1, num2, num1 - num2);
        sprintf(res, "%d - %d = %d\n", num1, num2, num1 - num2);
        copy_to_user(u, res, strlen(res));
    }
    return 0;
}

static ssize_t drv_write(struct file* fp, const char __user * u, size_t sz, loff_t* loff){
    printk(KERN_EMERG "hello write.\n");
    copy_from_user(xzc, u, sz);
    return 0;
}

static struct file_operations drv_ops = 
{
    .owner  =   THIS_MODULE,
    .open   =   drv_open,    
    .read   =   drv_read, 
    .write  =   drv_write,
};

static int xuzichuanDrv_init(void)
{
    int ret;
    cdev_init(&drv, &drv_ops);
    ret = alloc_chrdev_region(&ndev, 0, 1, "xuzichuanDrv");
    if (ret < 0)
    {
        printk(KERN_EMERG " alloc_chrdev_region error.\n");
        return ret;
    }
    printk(KERN_EMERG "xuzichuanDrv_init(): major = %d, minor = %d\n", MAJOR(ndev), MINOR(ndev));
    ret = cdev_add(&drv, ndev, 1);
    if (ret < 0)
    {
        printk(KERN_EMERG " cdev_add error.\n");
        return ret;
    }
    return 0;
}

static void xuzichuanDrv_exit(void)
{
    printk("exit process!\n");
	cdev_del(&drv);
	unregister_chrdev_region(ndev, 1);
}

module_init(xuzichuanDrv_init);
module_exit(xuzichuanDrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("u201912345@hust.edu.cn");
MODULE_DESCRIPTION("XZC's Driver Demo");