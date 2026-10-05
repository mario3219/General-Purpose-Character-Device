#include <linux/compiler.h>
#include <linux/container_of.h>
#include <linux/jiffies.h>
#include <linux/kfifo.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/wait.h>

#include "../include/stream_driver.h"

// The *timer was given when the timer was first initialized
void stream_timer_callback(struct timer_list *timer) {
	struct stream_device *sdev;
	unsigned long flags;
	u32 sample;

	sdev = container_of(timer, struct stream_device, timer);

	sample = sdev->sample_counter++;

  /* spin_lock reserves access to the current processor unit. Essentially, one thread that operates and asks for access to the
   * sdev-fifo_lock, sees that it's unlocked, then it locks it and proceeds in the code. If another thread asks for access
   * and it's locked, then the process waits at that line, using while(spin_lock), thus "spinning" in the loop.
   */
	spin_lock_irqsave(&sdev->fifo_lock, flags);

  // kfifo_avail returns how many bytes that are available.
	if (kfifo_avail(&sdev->fifo) >= sizeof(sample))
		kfifo_in(&sdev->fifo, &sample, sizeof(sample));

	spin_unlock_irqrestore(&sdev->fifo_lock, flags);

  /* This wakes up processes that are associated with read_queue. Earlier in probe(), we initialized
   * read_queue() with sdev. Later in read(), we will put the process to sleep and associate it with
   * the read_queue.
   */
	wake_up_interruptible(&sdev->read_queue);

  // Checks the running boolean if the timer should be reset
	if (READ_ONCE(sdev->running)) {
		mod_timer(
			&sdev->timer,
			jiffies + msecs_to_jiffies(100)
		);
	}
}
