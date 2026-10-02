#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init ecg_init(void)
{
    pr_info("ECG driver: initialized\n");
    return 0;
}

static void __exit ecg_exit(void)
{
    pr_info("ECG driver: removed\n");
}

module_init(ecg_init);
module_exit(ecg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonathan");
MODULE_DESCRIPTION("Virtual ECG acquisition driver");
