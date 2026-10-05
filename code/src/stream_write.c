#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

#include "../include/stream_driver.h"

ssize_t stream_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    struct stream_file *state = file->private_data;
    struct stream_device *sdev = state->sdev;

    unsigned char *tmp;
    unsigned long flags;
    unsigned int copied;

    if (count == 0)
        return 0;

    tmp = kmalloc(count, GFP_KERNEL);
    if (!tmp)
        return -ENOMEM;

    if (copy_from_user(tmp, buf, count)) {
        kfree(tmp);
        return -EFAULT;
    }

    spin_lock_irqsave(&sdev->fifo_lock, flags);

    copied = kfifo_in(&sdev->fifo, tmp, count);

    spin_unlock_irqrestore(&sdev->fifo_lock, flags);

    kfree(tmp);

    if (copied > 0)
        wake_up_interruptible(&sdev->read_queue);

    return copied;
}
