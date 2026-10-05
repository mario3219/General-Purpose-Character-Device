#ifndef STREAM_DRIVER_H
#define STREAM_DRIVER_H

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/wait.h>

struct class;
struct device;
struct platform_device;

#define DRIVER_NAME "stream-device"
#define DEVICE_NAME "streamdev"
#define FIFO_SIZE 4096

struct stream_device {
	struct device *dev;

	dev_t devno;
	struct cdev cdev;
	struct class *class;

	struct kfifo fifo;
	spinlock_t fifo_lock;
	struct mutex config_lock;

	wait_queue_head_t read_queue;

	struct timer_list timer;

	u32 sample_counter;
	bool running;
};

struct stream_file {
	struct stream_device *sdev;
	u64 bytes_read;
};

extern const struct file_operations stream_fops;

int stream_probe(struct platform_device *pdev);
void stream_remove(struct platform_device *pdev);

void stream_timer_callback(struct timer_list *timer);

int stream_open(struct inode *inode, struct file *file);
int stream_release(struct inode *inode, struct file *file);
ssize_t stream_read(struct file *file, char __user *buf, size_t count, loff_t *offset);

__poll_t stream_poll(struct file *file, struct poll_table_struct *wait);

#endif
