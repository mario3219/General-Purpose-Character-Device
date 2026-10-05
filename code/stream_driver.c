#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include "include/stream_driver.h"

/*
  PRIVATE NOTES:

  stream_driver is a struct of type platform_driver, which is a linux specific format for declaring platform drivers. The struct describes syscalls and what functions to use once they are called. When the driver is loaded, it finds the associated driver name from DRIVER_NAME, and will find the stream_device that is also loaded. Then probe() will be called. Probe initializes the driver and device and initalizes (in this case, a simulated hardware) a timer.

  The timer is used to simulate sampling frequency. Every 100ms, a sample is collected (in reality generated, so a realistic case would be for the actual hardware to deliver samples, not an internal timer).

  The timer still runs after probe()! In probe(), stream_timer_callback was made to be the function to call once the timer expires. This function generates and stores data and also wakes up processes that are set to sleep using 

  Then, the userspace calls open(). There the struct file object (which gets created when user calls open()) gets assigned to point to the character device. In our case however, we also create unique states in which the file objects instead point to.

  Currently, the only unique states that are being tracked are the amount of bytes being read. Other functionalities can be expanded in stream_file in the header.

  Eventually, the user will call poll() to ask if there is more data available. This function will either put the call in a waiting queue or not. If the poll() pauses the execution, the userspace program will be put to sleep. This is a solution to circumvent solutions such as while loops to repeatedly check conditions.

  If the poll() succeeds, the user can call read(). Inside read(), we also have non-block behavior that exits the call if there
  is no new data. If no non-block behavior is specified by the user, a waiting queue is issued instead, and the thread sleeps.

  The remaining syscalls are close() and release(). Release is tied to userspace open(). When user calls close(fd), release() gets called. Remove is tied to the platform device. If the module is unloaded using rmmod or similar, remove() gets called. The kernel first executes the driver specific remove() call, and then performs kernel specific logic to unload the module, in which the remaining memory allocation gets released.

*/

const struct file_operations stream_fops = {
	.owner   = THIS_MODULE,
	.open    = stream_open,
	.release = stream_release,
	.read    = stream_read,
  .write   = stream_write,
	.poll    = stream_poll,
};

static struct platform_driver stream_driver = {
	.probe  = stream_probe,
	.remove = stream_remove,

	.driver = {
		.name = DRIVER_NAME,
	},
};

module_platform_driver(stream_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonathan Olsson");
MODULE_DESCRIPTION(
	"Virtual streaming platform driver"
);
