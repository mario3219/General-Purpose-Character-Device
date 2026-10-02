#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static dev_t ecg_dev;
static struct cdev ecg_cdev;

static int ecg_open(struct inode *inode, struct file *file)
{
    pr_info("ECG driver: device opened\n");
    return 0;
}

static ssize_t ecg_read(struct file *file,
                        char __user *buffer,
                        size_t count,
                        loff_t *offset)
{
    s16 sample = 1234;

    if (*offset > 0)
        return 0;

    if (count < sizeof(sample))
        return -EINVAL;

    if (copy_to_user(buffer, &sample, sizeof(sample)))
        return -EFAULT;

    *offset += sizeof(sample);

    return sizeof(sample);
}

static const struct file_operations ecg_fops = {
    .owner = THIS_MODULE,
    .open = ecg_open,
    .read = ecg_read,
};

static int __init ecg_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&ecg_dev, 0, 1, "ecg");

    if (ret < 0) {
        pr_err("ECG driver: failed to allocate device number\n");
        return ret;
    }

    cdev_init(&ecg_cdev, &ecg_fops);

    ret = cdev_add(&ecg_cdev, ecg_dev, 1);

    if (ret < 0) {
        pr_err("ECG driver: failed to add cdev\n");
        unregister_chrdev_region(ecg_dev, 1);
        return ret;
    }

    pr_info("ECG driver: initialized\n");
    pr_info("ECG driver: major=%d minor=%d\n",
            MAJOR(ecg_dev), MINOR(ecg_dev));

    return 0;
}

static void __exit ecg_exit(void)
{
    cdev_del(&ecg_cdev);
    unregister_chrdev_region(ecg_dev, 1);

    pr_info("ECG driver: removed\n");
}

module_init(ecg_init);
module_exit(ecg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonathan");
MODULE_DESCRIPTION("Virtual ECG acquisition driver");
