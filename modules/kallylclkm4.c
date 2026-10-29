#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/panic.h>

static int __init kallylc_init(void) {
    pr_alert("Triggering Kernel PANIC...\n");
    int lincrashval = 1;
    panic("KallyLC Intentional Kernel Panic");
    return -EINVAL;
}

static void __exit kallylc_exit(void) {
    pr_info("Unloaded!\n");
}

module_init(kallylc_init);
module_exit(kallylc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>");

