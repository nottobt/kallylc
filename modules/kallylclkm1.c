#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>

static int __init kallylc_init(void) {
    pr_alert("Triggering Kernel WARN...\n");
    int lincrashval = 1;
    WARN(lincrashval == 1, "KallyLC Intentional Warning");
    return -EINVAL;
}

static void __exit kallylc_exit(void) {
    pr_info("Unloaded!\n");
}

module_init(kallylc_init);
module_exit(kallylc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>");
