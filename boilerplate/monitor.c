



/*   - device registration and teardown
 *   - timer setup
 *   - WRSS helper
 *   - soft-limit and hard-limit event helpers
 *   - ioctl dispatch shell
 *
 * YOUR WORK: Fill in all sections marked // TODO.

 */
   // for copy_from_user

    // for printk
#include <linux/errno.h>     // for EINVAL
#include "monitor_ioctl.h"
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include "monitor_ioctl.h"

#define DEVICE_NAME "container_monitor"
#define CHECK_INTERVAL_SEC 1

/* =
=============================================================
 * TODO 1: Define your linked-list node struct.
 *
 * Requirements:
 *   - track PID, container ID, soft limit, and hard limit
 *   - remember whether the soft-limit warning was already emitted
 *   - include `struct list_head` linkage
 * ============================================================== */


/* ==============================================================
 * TODO 2: Declare the global monitored list and a lock.
 *
 * Requirements:
 *   - shared across ioctl and timer code paths
 *   - protect insert, remove, and iteration safely
 *
 * You may choose either a mutex or a spinlock, but your README must
 * justify the choice in terms of the code paths you implemented.
 * ============================================================== */

#include <linux/list.h>
#include <linux/spinlock.h>

struct monitored_proc {
    pid_t pid;
    unsigned long soft_limit;
    unsigned long hard_limit;
    int warned;

    struct list_head list;
};


static LIST_HEAD(proc_list);
static struct task_struct *monitor_thread;
/* --- Provided: internal device / timer state --- */
static struct timer_list monitor_timer;
static dev_t dev_num;
static struct cdev c_dev;
static struct class *cl;
static spinlock_t proc_lock;
/* ---------------------------------------------------------------
 * Provided: RSS Helper
 *
 * Returns the Resident Set Size in bytes for the given PID,
 * or -1 if the task no longer exists.
 * --------------------------------------------------------------- */
__attribute__((unused))
static long  get_rss_bytes(pid_t pid)
{
    struct task_struct *task;
    struct mm_struct *mm;
   long  rss_pages = 0;

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        rcu_read_unlock();
        return -1;
    }
    get_task_struct(task);
    rcu_read_unlock();

    mm = get_task_mm(task);
    if (mm) {
        rss_pages = get_mm_rss(mm);
        mmput(mm);
    }
    put_task_struct(task);

    return rss_pages * PAGE_SIZE;
}

/* ---------------------------------------------------------------
 * Provided: soft-limit helper
 *
 * Log a warning when a process exceeds the soft limit.
 * --------------------------------------------------------------- */
__attribute__((unused))
static void log_soft_limit_event(const char *container_id,
                                 pid_t pid,
                                 unsigned long limit_bytes,
                                 long rss_bytes)
{
    printk(KERN_WARNING
           "[container_monitor] SOFT LIMIT container=%s pid=%d rss=%ld limit=%lu\n",
           container_id, pid, rss_bytes, limit_bytes);
}

/* ---------------------------------------------------------------
 * Provided: hard-
limit helper
 *
 * Kill a process when it exceeds the hard limit.
 * --------------------------------------------------------------- */
__attribute__((unused))
static void kill_process(const char *container_id,
                         pid_t pid,
                         unsigned long limit_bytes,
                         long rss_bytes)
{
    struct task_struct *task;

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (task)
        send_sig(SIGKILL, task, 1);
    rcu_read_unlock();

    printk(KERN_WARNING
           "[container_monitor] HARD LIMIT container=%s pid=%d rss=%ld limit=%lu\n",
           container_id, pid, rss_bytes, limit_bytes);
}

/* ---------------------------------------------------------------
 * Timer Callback - fires every CHECK_INTERVAL_SEC seconds.
 * --------------------------------------------------------------- */
static void timer_callback(struct timer_list *t)
{
    struct monitored_proc *p;
    struct monitored_proc *tmp;
    long rss;

    printk(KERN_INFO "TIMER RUNNING\n");

    spin_lock(&proc_lock);

    list_for_each_entry_safe(p, tmp, &proc_list, list) {

        struct task_struct *task;

        // Check if process exists
        task = pid_task(find_vpid(p->pid), PIDTYPE_PID);

        if (!task) {
            printk(KERN_INFO "Removing dead process pid=%d\n", p->pid);
            list_del(&p->list);
            kfree(p);
            continue;
        }

        printk(KERN_INFO "CHECKING PROCESS pid=%d\n", p->pid);

        rss = get_rss_bytes(p->pid);

        if (rss < 0)
            continue;

        printk(KERN_INFO "LIMIT CHECK pid=%d rss=%ld\n", p->pid, rss);

        // SOFT LIMIT
        if (rss > p->soft_limit && !p->warned) {
            printk(KERN_INFO "[container_monitor] SOFT LIMIT exceeded: pid=%d rss=%ld\n", p->pid, rss);
            p->warned = 1;
        }

        // HARD LIMIT
        if (rss > p->hard_limit) {
            printk(KERN_INFO "[container_monitor] HARD LIMIT exceeded: pid=%d rss=%ld — killing\n", p->pid, rss);
            send_sig(SIGKILL, task, 0);

            list_del(&p->list);
            kfree(p);
        }
    }

    spin_unlock(&proc_lock);

    mod_timer(&monitor_timer, jiffies + CHECK_INTERVAL_SEC * HZ);
}/*     * TODO 3: Implement periodic monitoring.
     *$
     * Requirements:
     *   - iterate through tracked entries safely
     *   - remove entries for exited processes
     *   - emit soft-limit warning once per entry
     *   - enforce hard limit and then remove the entry
     *   - avoid use-after-free while deleting during iteration
     * ============================================================== */


/* ---------------------------------------------------------------
 * IOCTL Handler
 *     *   - emit soft-limit warning once per entry
     *   - enforce hard limit and then remove the entry

 * Supported operations:
 *   - register a PID with soft + hard limits
 *   - unregister a PID when the runtime no longer needs tracking
 * --------------------------------------------------------------- */
static long monitor_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{   printk(KERN_INFO "IOCTL CALLED\n");
    struct monitor_request req;

    (void)f;

    if (cmd != MONITOR_REGISTER && cmd != MONITOR_UNREGISTER)
        return -EINVAL;

    if (copy_from_user(&req, (struct monitor_request __user *)arg, sizeof(req))) {
    return -EFAULT;
}
        

    if (cmd == MONITOR_REGISTER) {
        printk(KERN_INFO "IOCTL REGISTER CALLED\n");
        printk(KERN_INFO
               "[container_monitor] Registering container=%s pid=%d soft=%lu hard=%lu\n",
               req.container_id, req.pid, req.soft_limit_bytes, req.hard_limit_bytes);
       struct monitored_proc *p;

p = kmalloc(sizeof(*p), GFP_KERNEL);
if (!p)
    return -ENOMEM;

p->pid = req.pid;
p->soft_limit = req.soft_limit_bytes;
p->hard_limit = req.hard_limit_bytes;
p->warned = 0;
INIT_LIST_HEAD(&p->list);
/* add to list */
spin_lock(&proc_lock);
list_add(&p->list, &proc_list);
spin_unlock(&proc_lock);

        /* ==============================================================
         * TODO 4: Add a monitored entry.
         *
         * Requirements:
         *   - allocate and initialize one node from req
         *   - validate allocation and limits
         *   - insert into the shared list under the chosen lock
         * ============================================================== */

        return 0;
    }

    printk(KERN_INFO
           "[container_monitor] Unregister request container=%s pid=%d\n",
           req.container_id, req.pid);

    /* ==============================================================
     * TODO 5: Remove a monitored entry on explicit unregister.
     *
     * Requirements:
     *   - search by PID, container ID, or both
     *   - remove the matching entry safely if found
     *   - return status indicating whether a matching entry was removed
     * ============================================================== */

    return -ENOENT;
}
/* --- Provided: file operations --- */
static struct 
file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = monitor_ioctl,
};

/* --- Provided: Module Init --- */
static int __init monitor_init(void)
{
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0)
        return -1;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cl = class_create(DEVICE_NAME);
#else
    cl = class_create(THIS_MODULE, DEVICE_NAME);
#endif
    if (IS_ERR(cl)) {
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(device_create(cl, NULL, dev_num, NULL, DEVICE_NAME))) {
        class_destroy(cl);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, dev_num, 1) < 0) {
        device_destroy(cl, dev_num);
        class_destroy(cl);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }
    spin_lock_init(&proc_lock);
    timer_setup(&monitor_timer, timer_callback, 0);
    mod_timer(&monitor_timer, jiffies + CHECK_INTERVAL_SEC * HZ);

    printk(KERN_INFO "[container_monitor] Module loaded. Device: /dev/%s\n", DEVICE_NAME);
    return 0;
}

/* --- Provided: Module Exit --- */
static void __exit monitor_exit(void)
{
    del_timer_sync(&monitor_timer);

    /* ==============================================================
     * TODO 6: Free all remaining monitored entries.
     *
     * Requirements:
     *   - remove and free every list node safely
     *   - leave no leaked state on module unload
     * ============================================================== */

    cdev_del(&c_dev);
    device_destroy(cl, dev_num);
    class_destroy(cl);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "[container_monitor] Module unloaded.\n");
}

module_init(monitor_init);
module_exit(monitor_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Supervised multi-container memory monitor");
