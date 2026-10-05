#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "../include/stream_driver.h"

// Only the per-open state gets destroyed
int stream_release(struct inode *inode, struct file *file) {
	struct stream_file *state = file->private_data;

	if (state) {
		dev_info(
			state->sdev->dev,
			"device closed: %llu bytes read\n",
			state->bytes_read
		);

		kfree(state);
	}

	return 0;
}
