#ifndef MYDEBUG_H
#define MYDEBUG_H

#define DCRITICAL 0
#define DERROR 1
#define DIMPORTANT 2
#define DINFO 3
#define DVERBOSE 4

//Interface:
// MYDBG(level,fmt,args...) // Print msg "args" if appropriate debugging level
// MYDBGL(level,fmt,args...) // L = Precede "args" with Line number
// MYDBGLF(level,fmt,args...) // LF = Precede "args" with Line number and File name
// Note that even though gcc may says 3 args required, 2 args (level and fmt) will suffice

//For kernel land, also adds a simple assert() call.  Since can't call exit or
//abort, just returns from the current function with value -EFAULT.


#ifdef __KERNEL__
/***************************************************************************
  KERNEL VERSION
 **************************************************************************/
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */


#define MYDBG(level,fmt,args...) {if (level <= currlevel) printk(KERN_INFO fmt, ##args);}
#define MYDBGL(level,fmt,args...) {if (level <= currlevel) printk(KERN_INFO "%d " fmt, __LINE__, ##args);}
#define MYDBGLF(level,fmt,args...) {if (level <= currlevel) printk(KERN_INFO "%s:%d " fmt, __FILE__, __LINE__, ##args);}

/* Change last letter of macro and first letter of currlevel to make new logger */
#define MYDBGM(level,fmt,args...) {if (level <= currlevel) printk(KERN_INFO "M:"fmt, ##args);}
#define MYDBGML(level,fmt,args...) {if (level <= currlevel) printk(KERN_INFO "M:%d " fmt, __LINE__, ##args);}
#define MYDBGMLF(level,fmt,args...) {if (level <= currlevel) printk(KERN_INFO "M:%s:%d " fmt, __FILE__, __LINE__, ##args);}

/* #define MYDBG(level,msg) {if (level <= currlevel) printk(KERN_INFO msg);} */
/* #define MYDBGM(level,msg) {if (level <= Mcurrlevel) printk(KERN_INFO msg);} */


//KERNEL VERSION.  User version should just call exit or abort

//Could change to call panic(), but I don't think I want that generally.

//This syntax also works for string asserts,
//e.g., myassert(!"This code shouldn't be reached.")
#define myassert(cond) { \
    if (!(cond)) {							\
      printk(KERN_ALERT "***********\n");				\
      printk(KERN_ALERT "* Assertion %s failed!\n", #cond);		\
      printk(KERN_ALERT "* Location: %s:%d \n", __FILE__ , __LINE__);	\
      printk(KERN_ALERT "***********\n");				\
      printk(KERN_INFO "***********\n");				\
      printk(KERN_INFO "* Assertion %s failed!\n", #cond);		\
      printk(KERN_INFO "* Location: %s:%d \n", __FILE__ , __LINE__);	\
      printk(KERN_INFO "***********\n");				\
      return -EFAULT;							\
    }									\
}


extern int currlevel;
/*Add a new int with own first letter here when adding new logger.*/
/*Best to initialize somewhere to set default value.*/
extern int Mcurrlevel;


#else  //i.e., not __KERNEL__
/***************************************************************************
  USERLAND VERSION
 **************************************************************************/


#include <stdio.h>
#include <assert.h>

#define MYDBG(level,fmt,args...) {if (level <= currlevel) printf(fmt, ##args);}
#define MYDBGL(level,fmt,args...) {if (level <= currlevel) printf("%d " fmt, __LINE__, ##args);}
#define MYDBGLF(level,fmt,args...) {if (level <= currlevel) printf("%s:%d " fmt, __FILE__, __LINE__, ##args);}

/* Change last letter of macro and first letter of currlevel to make new logger */
#define MYDBGM(level,fmt,args...) {if (level <= Mcurrlevel) printf("M " fmt, ##args);}

/* #define MYDBG(level,msg) {if (level <= currlevel) printf(msg);} */
/* #define MYDBGM(level,msg) {if (level <= Mcurrlevel) printf(msg);} */


extern int currlevel;
/*Add a new int with own first letter here when adding new logger.*/
/*Best to initialize somewhere to set default value.*/
extern int Mcurrlevel;

#endif //__KERNEL__

#endif //MYDEBUG_H

