#include <linux/container_of.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "../include/stream_driver.h"

// inode represents the filesystem/device node, this case being /dev/streamdev0
int stream_open(struct inode *inode, struct file *file) {
	struct stream_device *sdev;
	struct stream_file *state;

  // i_cdev points to cdev, so essentially we are retrieving the container of cdev, which is sdev
	sdev = container_of(
		inode->i_cdev,
		struct stream_device,
		cdev
	);

  // Allocate data for the state, described in stream_driver.h that holds unique reader states to the kfifo queue
  // We created a stream_file pointer before called state, and we allocated memory to it.
	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

  // The state character device points to the character device
	state->sdev = sdev;
	state->bytes_read = 0;

  // This is important. When a user runs open(), a struct file object is created. It is through this object that communication is occuring.
  // Here in this line, we assign the private_data to point to the state, which holds information unique to the opened file instance
	file->private_data = state;

	dev_info(
		sdev->dev,
		"device opened\n"
	);
	return 0;
}
