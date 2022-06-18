#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/delay.h>


MODULE_LICENSE("GPL");

#define DEVICE_NUM  234
#define DEVICE_NAME "mydev"
#define BUFFER_SIZE 64

DEFINE_KFIFO(FIFO_BUFFER,char,BUFFER_SIZE);

// 实际从缓冲区读取的数据 real read
unsigned int rRead;  
// 实际向缓冲区输入的数据 real write
unsigned int rWrite; 

//读队列 read queue
wait_queue_head_t rQueue;  
//写队列 write queue
wait_queue_head_t wQueue; 

// 初始化操作
static int __init devInit(void){
    int ret;
    
    ret = register_chrdev(DEVICE_NUM,DEVICE_NAME, &devModule);   
    if (ret < 0) {
      printk(KERN_EMERG DEVICE_NAME "ERROR!\n");
      return ret;
    }
    
    printk(KERN_EMERG DEVICE_NAME "Initialization succeeded!\n");
    //初始化读队列
    init_waitqueue_head(&rQueue);  
    //初始化写队列    
    init_waitqueue_head(&wQueue);  
    return 0;
}


static void __exit devExit(void){
    unregister_chrdev(DEVICE_NUM, DEVICE_NAME);
    printk(KERN_EMERG DEVICE_NAME "Remove succeeded.\n");
}

//kfifo结构 读
static ssize_t devRead(struct file *file, char __user * buf, size_t count, loff_t *ppos)  
{
    //如果缓冲区是空的
    if(kfifo_is_empty(&FIFO_BUFFER))   
    {
        //f_flags: 指定打开该文件的方式
        //O_NONBLOCK: 是否是非堵塞式的操作 
        if(file->f_flags&O_NONBLOCK)  
            //read操作没有数据可读,返回一个错误EAGAIN
            return -EAGAIN;           
            
            //缓冲区不空 唤醒rQueue
        wait_event_interruptible(rQueue,!kfifo_is_empty(&FIFO_BUFFER));  
    } 
     //缓冲区不空 开始读数据 存在rRead中
    kfifo_to_user(&FIFO_BUFFER,buf,count,&rRead); 
     //如果缓冲区没有满 唤醒wQueue
    if(!kfifo_is_full(&FIFO_BUFFER)) 
    {
        wake_up_interruptible(&wQueue);   
    }
    //返回实际读取的数据的大小
    return rRead;  
}

static ssize_t devWrite(struct file *file,const char __user * buf, size_t count, loff_t *ppos)  //写函数
{
    // 判断缓冲区是否满 
    // 当缓冲区满的时候 只能读
    if(kfifo_is_full(&FIFO_BUFFER))  
    {
        //非阻塞状态
        if(file->f_flags&O_NONBLOCK)  
            return -EAGAIN;   
        
         //缓冲区未满的时候 唤醒wQueue
        wait_event_interruptible(wQueue,!kfifo_is_full(&FIFO_BUFFER)); 
    }
    
     //缓冲区不满就可以写数据 
    kfifo_from_user(&FIFO_BUFFER,buf,count,&rWrite);
     //如果缓冲区中有数据
    if(!kfifo_is_empty(&FIFO_BUFFER)) 
        //唤醒读队列 可以从缓冲区中取出数据
        wake_up_interruptible(&rQueue); 
    //返回实际写入的数据的大小
    return rWrite;  
}

 //定义写和读函数
static struct file_operations devModule = {  
    .owner  =   THIS_MODULE,
    .write  =   devWrite,
    .read   =   devRead,
};



module_init(devInit);
module_exit(devExit);


MODULE_AUTHOR("xzc");
MODULE_DESCRIPTION("task2");

