#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kdev_t.h>
#include <linux/kfifo.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/wait.h>

#include "../include/stream_driver.h"

/*
 * stream_probe will be called and given pdev, which is a pointer to the platform device the kernel found. A temporary struct for our stream_device, described in stream_driver.h and holds all relevant variables for our driver, is initialized as an empty struct.
 */

int stream_probe(struct platform_device *pdev) {
	struct stream_device *sdev;
	int ret;

  // Simple print of the device info
	dev_info(&pdev->dev, "platform device matched, probing\n");

  // Here memory is allocated with the same space as sdev and its variables. pdev->dev becomes the owner, but sdev points to the same space.
	sdev = devm_kzalloc(&pdev->dev, sizeof(*sdev), GFP_KERNEL);

  // Checks if the allocation succeeded
	if (!sdev)
		return -ENOMEM;

  // The memory allocation was successfull. The internal stream device can now point to the same platform device safely
	sdev->dev = &pdev->dev;

  // Variables associated with the stream_device struct gets initialized
	spin_lock_init(&sdev->fifo_lock);
	mutex_init(&sdev->config_lock);
	init_waitqueue_head(&sdev->read_queue);

  // The character device fifo gets memory allocated
	ret = kfifo_alloc(&sdev->fifo, FIFO_SIZE, GFP_KERNEL);

	if (ret)
		return ret;

  // If the fifo allocation succeeds, the device gets assigned a major and minor index number
	ret = alloc_chrdev_region(&sdev->devno, 0, 1, DEVICE_NAME);

  // We will see alot of goto functions. Essentially, ret holds an integer and gets updated and used as an error tracker.
  // goto means jumping to a line where the parameter is declared.
	if (ret)
		goto err_fifo;

  // Assigns file_operations described in stream_driver.h to the character device
	cdev_init(&sdev->cdev, &stream_fops);

	sdev->cdev.owner = THIS_MODULE;

  // Registers the character device in the kernel with the associated device numbers
	ret = cdev_add(&sdev->cdev, sdev->devno, 1);

	if (ret)
		goto err_chrdev;

  // Creates a linux class according to the linux device model
  // Two steps are needed: creating the device, creating the class
	sdev->class = class_create(DEVICE_NAME);

	if (IS_ERR(sdev->class)) {
		ret = PTR_ERR(sdev->class);
		goto err_cdev;
	}

  // The device is created. Linux treats access to data streams as file locations. Here it creates the access point in /dev/streamdev0
	if (IS_ERR(device_create(sdev->class, &pdev->dev, sdev->devno, NULL, "streamdev0"))) {
		ret = -EINVAL;
		goto err_class;
	}

  // Previously, we associated our internal sdev with pdev->dev. Now we do the opposite way, so that pdev->sdev.
	platform_set_drvdata(pdev, sdev);

	dev_info(&pdev->dev, "created /dev/streamdev0 major=%d minor=%d\n", MAJOR(sdev->devno), MINOR(sdev->devno));

  // Will get skipped if goto gets called
	return 0;

  // Error handling
  err_class:
    class_destroy(sdev->class);
  err_cdev:
    cdev_del(&sdev->cdev);
  err_chrdev:
    unregister_chrdev_region(sdev->devno, 1);
  err_fifo:
    kfifo_free(&sdev->fifo);
	return ret;
}
