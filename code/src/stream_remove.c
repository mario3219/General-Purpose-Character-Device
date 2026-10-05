#include <linux/cdev.h>
#include <linux/compiler.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/platform_device.h>
#include <linux/timer.h>

#include "../include/stream_driver.h"

/* We access the platform device, and have access to everything associated with it. From there,
 * we access the drive itself through it. We destroy everything step by step. Do notice however that some members
 * remain declared. The instance of stream_device remains in memory, but the resources allocated to the members are freed.
 * The remaining resources in stream_device gets freed once the kernel eventually unloads the driver, as part of the kzalloc call.
 */
void stream_remove(struct platform_device *pdev)
{
	struct stream_device *sdev;

  // remove() now has access to everything associated with the platform device
	sdev = platform_get_drvdata(pdev);

  // Just prints the device info along with a logging print
	dev_info(&pdev->dev, "removing driver\n");

	device_destroy(sdev->class, sdev->devno);
	class_destroy(sdev->class);
	cdev_del(&sdev->cdev);

	unregister_chrdev_region(sdev->devno, 1);

	kfifo_free(&sdev->fifo);
}
