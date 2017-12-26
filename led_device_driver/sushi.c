#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
MODULE_AUTHOR("Rena Shimoda");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
static struct class *cls=NULL;
static dev_t dev;
static struct cdev cdv;

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{

	int size = 0;
	char sushi[] = {0xF0,0x9F,0x8D,0xA3,0x0A};
	if(copy_to_user(buf + size,(const char *)sushi,sizeof(sushi))){
		printk(KERN_INFO "sushi : copy_to_user failed\n");
	return -EFAULT;
	}
	size += sizeof(sushi);
	return size;

}


static struct file_operations led_fops = {
        .owner = THIS_MODULE,
//        .write = sushi_read,
		.read = sushi_read
};

static int __init init_mod(void)
{
	int retval;
	retval =  alloc_chrdev_region(&dev, 0, 1, "sushi");
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
		return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
	cls = class_create(THIS_MODULE,"sushi");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "sushi%d",MINOR(dev));
	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	device_destroy(cls, dev);
	class_destroy(cls);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);

