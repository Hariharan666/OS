#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/slab.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/proc_fs.h>
#include<asm/uaccess.h>
#include<linux/ioctl.h>

#define MAJOR_NUMBER 61
#define MAX_SIZE 4194304
#define SCULL_IOC_MAGIC  'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC,   1) 

#define SCULL_IOC_MAGIC1 'p'
#define SCULL_COP _IOW(SCULL_IOC_MAGIC1,1,unsigned long)

#define SCULL_IOC_MAGIC2 'l'
#define SCULL_SEN _IOR(SCULL_IOC_MAGIC2,1,unsigned long)


#define SCULL_IOC_MAGIC3 'm'
#define SCULL_SENREC _IOWR(SCULL_IOC_MAGIC3,1,unsigned long)

char dev_msg[100] = {0};
char copydevmsg[100] = {0};


void copyarray(unsigned long arg )
{
  int index = 0;
  char *p = arg;
 while( *p ) {
   dev_msg[index] = *(p++);
   index++;}
   dev_msg[index++] = NULL;
   printk ( "Received this message in driver %s\n",dev_msg);
 
}



void sendarray(unsigned long arg, char dev_msg[] )
{
  int index = 0;
  char *p = arg;
 while( dev_msg[index] ) {
   p[index] = dev_msg[index];
   index++;}
  printk ( "Sent this message from driver %s\n",dev_msg);

  
}

void  sendreceivearray(unsigned long arg )
{


  int index  = 0;
  while( dev_msg[index] ) {
    copydevmsg[index] = dev_msg[index];
    index++;}
  
 copyarray(arg);
 sendarray(arg,copydevmsg);
}
 





unsigned long ioctl_example(struct file *filp, unsigned int cmd, unsigned long arg) 
{ 
     int err = 0, tmp; 
    int retval = 0; 
 /* 
     * extract the type and number bitfields, and don't decode 
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok() 
     */ 
       if ( !(_IOC_TYPE(cmd) == SCULL_IOC_MAGIC || _IOC_TYPE(cmd) == SCULL_IOC_MAGIC1 ||  _IOC_TYPE(cmd) == SCULL_IOC_MAGIC2 || _IOC_TYPE(cmd) == SCULL_IOC_MAGIC3) )return -ENOTTY;
       if ( _IOC_NR(cmd) > 1 )  return -ENOTTY;
 
     /* 
     * the direction is a bitmask, and VERIFY_WRITE catches R/W 
     * transfers. `Type' is user‐oriented, while 
     * access_ok is kernel‐oriented, so the concept of "read" and 
     * "write" is reversed 
     */ 
    if (_IOC_DIR(cmd) & _IOC_READ) {
 err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));} 
    if (_IOC_DIR(cmd) & _IOC_WRITE) 
 err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd)); 
    if (err) return -EFAULT;
    switch(cmd) { 
            case SCULL_HELLO: 
        printk(KERN_WARNING "hello\n"); 
        break;  
        case SCULL_COP:
       printk(KERN_WARNING "Copying\n");
        copyarray(arg);
        break;
        case SCULL_SEN:
        printk(KERN_WARNING "Sending\n");
        sendarray(arg,dev_msg);
        break;
        case SCULL_SENREC:
        printk(KERN_WARNING "Sending and receiving\n");
        sendreceivearray(arg);
        break;
      default:  /* redundant, as cmd was checked against MAXNR */ 
      return -ENOTTY;
    } 
    return retval; 
 
} 




long ioctl_write(struct file *filp, unsigned int cmd, unsigned long arg)
{
     int err = 0, tmp;
    int retval = 0;
 /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
       if ( _IOC_TYPE(cmd) != SCULL_IOC_MAGIC1 ) return -ENOTTY;
       if ( _IOC_NR(cmd) > 1 )  return -ENOTTY;

     /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user‐oriented, while
     * access_ok is kernel‐oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
 err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
   
 err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (err) return -EFAULT;
    switch(cmd) {
        case SCULL_COP:
        printk(KERN_WARNING "Copying\n");
        copyarray(arg);
        break;
      default:  /* redundant, as cmd was checked against MAXNR */
      return -ENOTTY;
    }
    return retval;

}

   




/* forward declaration */
int multibyte_open(struct inode *inode, struct file *filep);
int multibyte_release(struct inode *inode, struct file *filep);
ssize_t multibyte_read(struct file *filep, char *buf, size_t
count, loff_t *f_pos);
ssize_t multibyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos);
loff_t multibyte_lseek(struct file *filep,loff_t offset,int typ);
static void multibyte_exit(void);



/* definition of file_operation structure */
struct file_operations multibyte_fops = {
read:
multibyte_read,
write:
multibyte_write,
open:
multibyte_open,
release: 
multibyte_release,
llseek:
multibyte_lseek,
.unlocked_ioctl = ioctl_example

};



char * multibyte_data = NULL;
char * start = NULL;
int read = 0;
unsigned long bytes_read = 0;
unsigned long readptr = 0;
int multibyte_open(struct inode *inode, struct file *filep)
{
multibyte_data -= readptr;
readptr = 0;
return 0; // always successful
}

int multibyte_release(struct inode *inode, struct file *filep)
{
return 0; // always successful
}


ssize_t multibyte_read(struct file *filep, char *buf, size_t
count, loff_t *f_pos)
{
  int index = 0;
  if( (*multibyte_data) == NULL )
     return 0;
  //if ( (*f_pos) != 0 ) {
  while( (count > 0 ) && (index <= (*f_pos))){
     put_user ( *(multibyte_data++),buf++ );
     count--;
     readptr++;
     index++;
  }
 /* }
  else {
     while( ((multibyte_data[(*f_pos)]) != NULL)){
     printk( "Value %c",multibyte_data[(*f_pos)] );
     put_user ( (multibyte_data[(*f_pos)++]),buf++ );
     count--;
     readptr++;
     
  }

  }*/
  return readptr;
}

ssize_t multibyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos)
{
   int bc = 0;
   while( count > 0 )
   {
    multibyte_data[(*f_pos)] = buf[bc];
    count--;
    (*f_pos)++;
    if( (*f_pos) >  4194302 ) {
         // multibyte_data[(++*f_pos)] = NULL;
          break;
    }
    bc++;
   }
 //  multibyte_data[(*f_pos)] = NULL;
   bytes_read += bc;
    printk( "Number of bytes read %d",bytes_read);
   return bc;

      
}


loff_t multibyte_lseek(struct file *filep,loff_t offset,int typ)
{
 loff_t new_pos=0;
 printk( "typ value %d\n",typ);
 switch( typ )
 {
  case 0: /* SEEK_SET: */
  new_pos = offset;
  break;
  case 1: /* SEEK_CUR: */
  new_pos = filep->f_pos + offset;
  break;
  case 2: /* SEEK_END: */
  new_pos = bytes_read - offset;
  break;
 }          
 if( new_pos > bytes_read ) new_pos = bytes_read;
 if( new_pos < 0 ) new_pos = 0;
 filep->f_pos = new_pos;
 return new_pos; 

}

static int multibyte_init(void)
{
int result;
// register the device
result = register_chrdev(MAJOR_NUMBER, "multi",&multibyte_fops);

if (result < 0) {
return result;
}


// allocate multi  byte of memory for storage
// kmalloc is just like malloc, the second parameter is
// the type of memory to be allocated.
// To release the memory allocated by kmalloc, use kfree.

multibyte_data = kmalloc(sizeof(char)*4*1024*1024, GFP_KERNEL);
memset(multibyte_data,0,sizeof(char)*4*1024*1024);
start = multibyte_data;
if (!multibyte_data) {
multibyte_exit();
// cannot allocate memory
// return no memory error, negative signify a failure
return -ENOMEM;
}



// initialize the value to be X
multibyte_data[0] = 'X';
multibyte_data[1] = 0;
printk(KERN_ALERT "This is a multibyte device module\n");
return 0;
}
static void multibyte_exit(void)
{
// if the pointer is pointing to something
// free the memory and assign the pointer to NULL
//kfree(multibyte_data);
//multibyte_data = NULL;
// unregister the device
unregister_chrdev(MAJOR_NUMBER, "multibyte");
printk(KERN_ALERT "multibyte device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(multibyte_init);
module_exit(multibyte_exit);
