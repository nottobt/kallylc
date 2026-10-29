#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>

static int __init kallylc_init(void) {
    int zero = 0;
    int result;
    pr_alert("Triggering Kernel Oops...\n");
    result = 42 / zero;
    pr_alert("Result: %d\n", result);
    return 0;
}
static void __exit kallylc_exit(void) {
    pr_info("Unloaded!\n");
}
module_init(kallylc_init);
module_exit(kallylc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>");
