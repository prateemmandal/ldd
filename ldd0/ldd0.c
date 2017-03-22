#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

static int __init ldd0_init(void) {
  printk(KERN_INFO "ldd0 loaded.");
  return 0;
}

static void __exit ldd0_exit(void) {
  printk(KERN_INFO "ldd0 removed.");
}

module_init(ldd0_init);
module_exit(ldd0_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prateem Mandal <prateem@gmail.com>");
MODULE_DESCRIPTION("Test driver.");
