#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

#include "../include/stream_driver.h"

/*
 * Userspace for example creates char buffer[128], this is *buf. *offset is used for MMIO registers (not used in this case
 * because we are not manipulating directly with hardware)
 */

ssize_t stream_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
	struct stream_file *state = file->private_data;
	struct stream_device *sdev = state->sdev;

	unsigned long flags;  // flags is for the spin_lock
	unsigned int copied;  // stores how many bytes that came out of FIFO
	unsigned char *tmp;   // will point to a temporary kernel-space buffer
	int ret;

  /* If the kernel is empty, we check also if we opened the device with non-blocking behavior.
   * Essentially we are checking that if there is no data, we don't want the userspace program to hang until
   * data is available.
   */
	if (kfifo_is_empty(&sdev->fifo)) {
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

    /* If there was no non-block behavior specified, we instead issue a wait event. The condition becomes if the FIFO queue
     * is no longer empty.
     */
		ret = wait_event_interruptible(
			sdev->read_queue,
			!kfifo_is_empty(&sdev->fifo)
		);

		if (ret)
			return ret;
	}

  /* We temporarily allocate memory for a kernel buffer. The size of the allocation is count from the parameters. The user
   * calls read() to specify the size of the sample that it requests.
   */
	tmp = kmalloc(count, GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	spin_lock_irqsave(&sdev->fifo_lock, flags);

  // Removes up to count bytes into tmp. Remember that copied is unsigned int. copied holds how many bytes that were copied.
	copied = kfifo_out(&sdev->fifo, tmp, count);

	spin_unlock_irqrestore(&sdev->fifo_lock, flags);

  /* Both copies the tmp buffer to user buf and checks if the operation was successful. We copy "copied" amount of bytes from
   * tmp to buf. We use copied because if we tried to copy count amount of bytes when in reality less was copied, copy_to_user
   * would return the amount of bytes that failed to copy. If its nonzero, the if statement triggers.
   */
	if (copy_to_user(buf, tmp, copied)) {
		kfree(tmp);
		return -EFAULT;
	}

	kfree(tmp);
	state->bytes_read += copied;

	return copied;
}
