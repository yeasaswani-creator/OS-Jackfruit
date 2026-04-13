#include <linux/module.h>
#include <linux/kernel.h>

static int __init monitor_init(void) {
    printk(KERN_INFO "Container Monitor Loaded\n");
    return 0;
}

static void __exit monitor_exit(void) {
    printk(KERN_INFO "Container Monitor Unloaded\n");
}

module_init(monitor_init);
module_exit(monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Task 4 Kernel Monitor");
