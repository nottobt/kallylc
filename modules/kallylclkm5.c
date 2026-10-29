#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/sched/types.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("nottobt");
MODULE_DESCRIPTION("KallyLC LKM 5: Preemption (High Nice) & Out of memory");

static struct task_struct *preempt_thread;

static int preempt_fn(void *data) {
    size_t sizeal = 4 * 1024 * 1024;
    sched_set_fifo(current);

    while (!kthread_should_stop()) {
        ktime_t start = ktime_get();
        void *ptr = kmalloc(sizeal, GFP_KERNEL);
        s64 taken_ns = ktime_to_ns(ktime_sub(ktime_get(), start));
        if (!ptr) {
            pr_info("Allocation failed! Out of memory imminent?!\n");
            break;
        }
        pr_info("Kally allocated %zu bytes in %lld nanoseconds\n", sizeal, taken_ns);

    }
    return 0;
}

static int __init kallylc_init(void) {
    pr_info("Starting Kally LKM number 5\n");
    preempt_thread = kthread_run(preempt_fn, NULL, "kallylclkm5");
    if (IS_ERR(preempt_thread)) {
        return PTR_ERR(preempt_thread);
    }
    return 0;
}

static void __exit kallylc_exit(void) {
    pr_info("Unloaded!\n");
    if (preempt_thread) {
        kthread_stop(preempt_thread);
    }
}

module_init(kallylc_init);
module_exit(kallylc_exit);
