#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/poll.h>

#include "../include/stream_driver.h"

/*
 * The user will call poll(&pfd, 1, -1). -1 is timeout, 1 is the number of file descriptors.
 * poll_table_struct is the kernel internal object connecting the driver to the wait queue.
 * We previously initialized the driver with a wait queue, and it is this that is accessed.
 *
 * If the returned mask is 0, the result gets returned to the kernel's generic poll() (we don't see this)
 * It is THERE that the calling thread gets put to sleep. That thread is connected to the wait queue,
 * so in timer_callback where we interrupt and wake up, this thread gets awakened.
 */
__poll_t stream_poll(struct file *file, struct poll_table_struct *wait) {
	struct stream_file *state = file->private_data;
	struct stream_device *sdev = state->sdev;

  // Start with nothing is ready
	__poll_t mask = 0;

  // Register the wait queue
	poll_wait(file, &sdev->read_queue, wait);

  // Checks if the driver is ready. If it's not ready, mask remains equal to 0.
	if (!kfifo_is_empty(&sdev->fifo))
		mask |= POLLIN | POLLRDNORM;

	return mask;
}
