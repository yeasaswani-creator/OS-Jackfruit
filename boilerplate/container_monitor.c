#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/string.h>

static int __init container_monitor_init(void)
{
    struct task_struct *task;

    printk(KERN_INFO "Container Monitor Loaded\n");

    for_each_process(task) {
        unsigned long rss_kb = 0;

        if (task->mm) {
            rss_kb = get_mm_rss(task->mm) << (PAGE_SHIFT - 10);
        }

        // FILTER (only memory_hog process)
        if (strcmp(task->comm, "memory_hog") == 0) {
            printk(KERN_INFO "[container_monitor] PID=%d COMM=%s RSS=%lu KB\n",
                   task->pid, task->comm, rss_kb);
        }
    }

    return 0;
}

static void __exit container_monitor_exit(void)
{
    printk(KERN_INFO "Container Monitor Unloaded\n");
}

module_init(container_monitor_init);
module_exit(container_monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Container Monitor with RSS");
