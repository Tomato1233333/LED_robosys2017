#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/time.h>
MODULE_AUTHOR("Rena Shimoda");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
static struct class *cls=NULL;
static dev_t dev;
static struct cdev cdv;
static volatile u32 *gpio_base = NULL; //書いたとおりに動けよ→volatile

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
		char c;
		if(copy_from_user(&c,buf,sizeof(char)))
			return -EFAULT;
		if(c == '0'){//デバイスIDが0だったら
			gpio_base[10] = 1 << 25;//[10]はデータシートGPFSEL0の10番目
		}
		else if(c == '1'){
			gpio_base[7] = 1 << 25;
		}
		else if(c == '2'){
			int tomato1 = jiffies + 100;
			int tomato2 = jiffies + 1000;
			gpio_base[7] = 1 << 25;
			while(jiffies < tomato1);
			schedule();
			gpio_base[10] = 1 << 25;
			while(jiffies < tomato2)
			schedule();
			printk("time = %d 秒経ったよ!!\n",tomato2 - jiffies);
			gpio_base[7] = 1 << 25;
			}

		printk(KERN_INFO "receive %c\n",c);
//        printk(KERN_INFO "led_write is called\n");
        return 1; //読み込んだ文字数を返す（この場合はダミーの1）
}

static struct file_operations led_fops = {
        .owner = THIS_MODULE,
        .write = led_write
};

static int __init init_mod(void)
{
	int retval;
	retval =  alloc_chrdev_region(&dev, 0, 1, "myled");
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
		return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
	gpio_base = ioremap_nocache(0x3f200000,0xA0);
	const u32 led = 25; //25番にありますよ
	const u32 index = led/10;
	const u32 shift = (led%10)*3; //GPFSLOの2の5番目のピンですよ
	const u32 mask = ~(0x7 << shift); //32bit,1の羅列,25番目だけ0になる
	gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);
	
	cls = class_create(THIS_MODULE,"myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));
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

