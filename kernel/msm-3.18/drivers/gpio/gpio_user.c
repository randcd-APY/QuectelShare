#include <linux/fs.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

static struct miscdevice gpio_device;
static void __iomem *msm_pinctrl_regs;

static int gpio_user_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static long gpio_user_ioctl(struct file *filp, uint32_t cmd, ulong arg)
{	
	int dir, gpio, type;
	int cfg_val, inout_val;

	gpio = _IOC_NR(cmd);
	dir  = _IOC_DIR(cmd);
	type = _IOC_TYPE(cmd);

	switch (type) {
	case 'A':
		if (dir == _IOC_WRITE) {
			get_user(cfg_val, (int *)arg);
			writel_relaxed(cfg_val, msm_pinctrl_regs + 0x1000 * gpio);
		} else {
			cfg_val = readl_relaxed(msm_pinctrl_regs + 0x1000 * gpio);
			put_user(cfg_val, (int *)arg);
		}
		break;
	case 'B':
		if (dir == _IOC_WRITE) {
			get_user(cfg_val, (int *)arg);
			writel_relaxed(cfg_val << 1, msm_pinctrl_regs + 4 + 0x1000 * gpio);
		} else {
			inout_val = readl_relaxed(msm_pinctrl_regs + 4 + 0x1000 * gpio);
			put_user(inout_val, (int *)arg);
		}
		break;
	default:
		pr_err("%s invalid ioctl %u\n", __func__, cmd);
		return -EINVAL;
	}

	return 0;
}

static int gpio_user_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations gpio_user_fops = {
	.owner          = THIS_MODULE,
	.open           = gpio_user_open,
	.unlocked_ioctl	= gpio_user_ioctl,
	.release        = gpio_user_release,
};

static int gpio_user_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct device *dev = &pdev->dev;

	pr_err("%s: start\n", __func__);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	msm_pinctrl_regs = ioremap(res->start, resource_size(res));
	if (IS_ERR(msm_pinctrl_regs)) {
		dev_err(dev, "%s: ioremap failed", __func__);
		return -EINVAL;
	}

	gpio_device.minor = MISC_DYNAMIC_MINOR;
	gpio_device.name = "gpio-reg";
	gpio_device.fops = &gpio_user_fops;

	if (misc_register(&gpio_device)) {
		dev_err(dev, "%s: misc_register failed", __func__);
		return -EINVAL;
	}

	pr_err("%s: suecceeded\n", __func__);

	return 0;
}

static int gpio_user_remove(struct platform_device *pdev)
{
	misc_deregister(&gpio_device);
	iounmap(msm_pinctrl_regs);

	return 0;
}

static struct of_device_id gpio_user_match_table[] = {
	{ .compatible = "gpio-user-ctrl", },
	{ },
};

static struct platform_driver gpio_user_drv = {
	.driver = {
		.name = "gpio-user",
		.owner = THIS_MODULE,
		.of_match_table = gpio_user_match_table,
	},
	.probe = gpio_user_probe,
	.remove = gpio_user_remove,
};

static int gpio_user_init(void)
{
	return platform_driver_register(&gpio_user_drv);
}

static void gpio_user_exit(void)
{
	platform_driver_unregister(&gpio_user_drv);
}

module_init(gpio_user_init);
module_exit(gpio_user_exit);
MODULE_LICENSE("GPL v2");
