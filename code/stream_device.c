#include <linux/module.h>
#include <linux/platform_device.h>

#define DEVICE_NAME "stream-device"

static struct platform_device *stream_pdev;

static int __init stream_device_init(void)
{
	int ret;

	pr_info("stream_device: registering platform device\n");

	stream_pdev = platform_device_alloc(DEVICE_NAME, -1);
	if (!stream_pdev)
		return -ENOMEM;

	ret = platform_device_add(stream_pdev);
	if (ret) {
		platform_device_put(stream_pdev);
		return ret;
	}

	pr_info("stream_device: platform device registered\n");

	return 0;
}

static void __exit stream_device_exit(void)
{
	pr_info("stream_device: unregistering platform device\n");

	platform_device_unregister(stream_pdev);
}

module_init(stream_device_init);
module_exit(stream_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonathan Olsson");
MODULE_DESCRIPTION("Virtual stream platform device");
