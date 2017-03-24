#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

static dev_t first;
static struct cdev c_dev;
static struct class *cl;

static char c = 'x';

static int my_open(struct inode *i, struct file *f) {
  printk(KERN_INFO "Driver: open()\n");
  return 0;
}

static int my_close(struct inode *i, struct file *f) {
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
  printk(KERN_INFO "Driver: read()\n");
  if (*off == 0) {
    if (copy_to_user(buf, &c, 1) != 0)
      return -EFAULT;
    else {
      (*off)++;
      return 1;
    }
  }
  else
    return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
  printk(KERN_INFO "Driver: write()\n");
  if (copy_from_user(&c, buf + len - 1, 1) != 0)
    return -EFAULT;
  else
    return len;
}

static struct file_operations pugs_fops =
{
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};

static int __init pmchar0_init(void) {
  int ret;
  struct device *dev_ret;
  
  printk(KERN_INFO "pmchar0 loading.");
  if ((ret = alloc_chrdev_region(&first, 0, 1, "pmchar0")) < 0) {
    return ret;
  }
  printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(first), MINOR(first));
  if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))) {
    unregister_chrdev_region(first, 1);
    return PTR_ERR(cl);
  }
  if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "pmchar0null"))) {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return PTR_ERR(dev_ret);
  }
  cdev_init(&c_dev, &pugs_fops);
  if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return ret;
  }
  return 0;
}

static void __exit pmchar0_exit(void) {
  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  printk(KERN_INFO "pmchar0 removed.");
}

module_init(pmchar0_init);
module_exit(pmchar0_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prateem Mandal <prateem@gmail.com>");
MODULE_DESCRIPTION("Test characted driver.");
