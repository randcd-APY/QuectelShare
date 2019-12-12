#include <linux/fs.h>	
#include <linux/of.h>
#include <linux/io.h>	
#include <linux/delay.h>	
#include <linux/kernel.h>	
#include <linux/device.h>	
#include <linux/module.h>	
#include <linux/proc_fs.h>	
#include <asm/uaccess.h>	
#include <linux/platform_device.h>	
	
#define GPIO_PWM_CLOCK "gpio_clk_pwm"		
#define GCC_MM_GP0_M        0x54008	
#define GCC_MM_GP0_N        0x5400C	
#define GCC_MM_GP0_D        0x54010	
#define GCC_MM_GP0_CMD_RCGR 0x54000	
#define GCC_CAMSS_GP0_CBCR  0x54018	
#define GCC_MM_GP0_CFG_RCGR 0x54004	
	
#define GCC_MM_GP1_M        0x55008	
#define GCC_MM_GP1_N        0x5500C	
#define GCC_MM_GP1_D        0x55010	
#define GCC_MM_GP1_CMD_RCGR 0x55000	
#define GCC_CAMSS_GP1_CBCR  0x55018	
#define GCC_MM_GP1_CFG_RCGR 0x55004	
	
static u32 gpio_nr;	
static int suspend_flag = 0;	
	
static u8 pwm_reg[6];	
static u32 pwm_enable = 0;	
static u32 g_freq = 0, g_duty = 0;	
	
static void __iomem *base_clk;	
static void __iomem *base_gpio;
	
static u8 M, N, D;	
static u8 clk_src, clk_div;	
	
static int GCC_MM_GP_CMD_RCGR, GCC_MM_GP_CFG_RCGR, GCC_MM_GP_M, GCC_MM_GP_N, GCC_MM_GP_D, GCC_CAMSS_GP_CBCR;	
static u32 fin[4] = {800000000, 614400000, 19200000, 32768};	
static u32 clk_src_cfg[4] = {0x21 << 8, 0x22 << 8, 0x20 << 8, 0x26 << 8};	
static char *clk_src_name[4] = {"GPLL0_MAIN", "GPLL1_AUX", "cxo", "core_pi_sleep_clk"};	
static u32 clk_div_a[] = {	
    2,  2,  3,  4,  5,  6,  7,  8,	
    9,  10, 11, 12, 13, 14, 15, 16,	
    17, 18, 19, 20, 21, 22, 23, 24,	
    25, 26, 27, 28, 29, 30, 31, 32,	
};
	
	
static struct proc_dir_entry *gpio_clk_proc_entry;	
	
static void calculate_MND_reg_addr (void)	
{	
    GCC_MM_GP_CMD_RCGR = 0x54000 + (gpio_nr - 31) * 0x1000;	
    GCC_MM_GP_CFG_RCGR = 0x54004 + (gpio_nr - 31) * 0x1000;	
    GCC_MM_GP_M        = 0x54008 + (gpio_nr - 31) * 0x1000;	
    GCC_MM_GP_N        = 0x5400C + (gpio_nr - 31) * 0x1000;	
    GCC_MM_GP_D        = 0x54010 + (gpio_nr - 31) * 0x1000;	
    GCC_CAMSS_GP_CBCR  = 0x54018 + (gpio_nr - 31) * 0x1000;	
}
	
	
static int calculate_best_MND_value (int f, int duty)	
{	
    int fo, f_abs_tmp;	
    u32 f_abs_min = (u32)-1;	
    int i, j;	
    int mm, nn;	
	
    for (i = 0; i < ARRAY_SIZE(fin); i++) {	
        for (j = 0; j < ARRAY_SIZE(clk_div_a); j++) {
            for (mm = 1; mm < 256; mm++) {	
                for (nn = mm + 1; nn < 256; nn++) {	
                    fo = fin[i] * 2 / clk_div_a[j] / nn * mm;	
                    f_abs_tmp = fo - f >= 0 ? fo - f: f - fo;	
                    if (f_abs_tmp < f_abs_min) {	
                        f_abs_min = f_abs_tmp;	
                        M = mm;	
                        N = nn;	
                        clk_src = i;	
                        clk_div = j;	
                    }	
					}	
            }	
        }	
    }		
    if (f_abs_min == (u32)-1) {	
        pr_err("set MND value failed\n");	
        return -EINVAL;	
    }		
    D = N * duty / 100;	
    if (2 * D > 256) {	
        pr_err("set duty failed\n");	
        return -EINVAL;	
    }	
	
    return 0;	
}
		
static void gpio_config (uint32_t gpio, uint8_t func, uint8_t pull, uint8_t drvstr, uint32_t enable)	
{	
    uint32_t val = 0;	
	
    val |= pull;	
    val |= func << 2;	
    val |= drvstr << 6;	
    val |= enable << 9;		
    writel_relaxed(val, base_gpio + (gpio) * 0x1000);	
}
		
static void update_pwm_reg (void)	
{	
    gpio_config(gpio_nr, 2, 3, 3, 1);	
    writel_relaxed(clk_src_cfg[clk_src] | clk_div, base_clk + GCC_MM_GP_CFG_RCGR);	
    writel_relaxed(M, base_clk + GCC_MM_GP_M);	
    writel_relaxed(255 - (N - M), base_clk + GCC_MM_GP_N);	
    writel_relaxed(255 - 2 * D, base_clk + GCC_MM_GP_D);	
    writel_relaxed(0x03, base_clk + GCC_MM_GP_CMD_RCGR);	
    writel_relaxed(0x01, base_clk + GCC_CAMSS_GP_CBCR);	
}
	
	
static void pwm_output (int f, int duty)	
{	
    int ret;		
    if (f != g_freq || duty != g_duty) {	
        if (f != g_freq) {   // if freq was modified, then calculate MND prams	
            ret  = calculate_best_MND_value(f, duty);	
            if (ret) {	
                pr_err("set MND value failed\n");
	                return;	
            }	
        } else {   // only duty was modified, just need calculate D value	
            D = N * duty / 100;	
            if (2 * D > 256) {	
                pr_err("set duty failed\n");	
                return;	
            }	
        }	
        update_pwm_reg();	
        pwm_enable = 1;	
        g_freq = f;	
        g_duty = duty;		
        pr_err("---+++ clk src: %s, clk div: %d.%d, M: %d, N: %d, D: %d\n",	
            clk_src_name[clk_src], clk_div_a[clk_div] / 2, 	
            clk_div_a[clk_div] % 2 ? 5: 0, M, N, D);	
    } else	
        pr_err("---+++ pwm already enabled\n");	
}
		
static void pwm_reg_save_default (void)	
{	
    int i;	
	
    for(i = 0; i < 5; i++)	
        pwm_reg[i] = readl_relaxed(base_clk + GCC_MM_GP_CMD_RCGR + 4 * i);	
    pwm_reg[i] = readl_relaxed(base_clk + GCC_CAMSS_GP_CBCR);	
}	
	
static void pwm_reg_restore_default (void)	
{	
    int i;	
	
    for (i = 0; i < 5; i++)	
        writel_relaxed(pwm_reg[i], base_clk + GCC_MM_GP_CMD_RCGR + 4 * i);	
    writel_relaxed(pwm_reg[i], base_clk + GCC_CAMSS_GP_CBCR);	
    gpio_config(gpio_nr, 0, 1, 0, 0);	
    writel_relaxed(0, base_gpio + 0x00000004 + gpio_nr * 0x1000);	
}
		
static ssize_t gpio_clock_show (struct file *file, char __user *buf, size_t size, loff_t *ppos)	
{	
    int ret, len;	
    char kbuf[128];
		
    if (pwm_enable)	
        len = scnprintf(kbuf, PAGE_SIZE, 	
        "freq = %d, duty = %d %%\n"	
        "clk src: %s, clk div: %d.%d, M: %d, N: %d, D: %d\n",	
        g_freq, g_duty, 	
        clk_src_name[clk_src], clk_div_a[clk_div] / 2, 	
        clk_div_a[clk_div] % 2 ? 5: 0, M, N, D);	
    else	
        len = scnprintf(kbuf, PAGE_SIZE, "gpio clk disabled\n");	
        	
    ret = copy_to_user(buf, kbuf, len);	
    if (ret) {	
        pr_err("---+++ copy from user failed\n");	
        return 0;	
    }	
	
    return len;	
}
		
static ssize_t gpio_clock_store (struct file *filp, const char __user *buf, size_t len, loff_t *off)	
{	
    int i, ret;	
    char *pwm_cfg[2];	
    char kbuf[128];	
    char *kpbuf = kbuf;	
    ulong freq, duty;	
	
    memset(kbuf, '\0', sizeof(kbuf));	
    ret = copy_from_user(kbuf, buf, len);	
    if (ret) {	
        pr_err("---+++ copy from user failed\n");	
        return 0;	
    }	
    if (kbuf[0] == '0') { 	
        g_freq     = 0;	
        g_duty     = 0;	
        pwm_enable = 0;	
        pwm_reg_restore_default();	
        return len;	
    }	
	
    if (strchr(kbuf, ':') == NULL)	
        return len;
		
    for (i = 0; i < ARRAY_SIZE(pwm_cfg); i++)	
        pwm_cfg[i] = strsep((char **)&kpbuf, ":");	
	
    ret = kstrtoul(pwm_cfg[0], 10, &freq);	
    if (ret < 0) {	
		pr_err("---+++ get pwm freq failed\n");	
		return len;	
    }
		
    ret = kstrtoul(pwm_cfg[1], 10, &duty);	
    if (ret < 0 || duty >= 100) {	
		pr_err("---+++ get pwm duty failed\n");	
		return len;	
    }	
	
    pwm_output(freq, duty);	
    return len;
	
}	
	
static const struct file_operations gpio_clk_proc_ops = {	
    .owner = THIS_MODULE,	
    .read = gpio_clock_show,	
    .write = gpio_clock_store,	
};
		
static int gpio_clk_probe (struct platform_device *pdev)	
{	
    int ret, freq, duty;	
	struct device_node *pnode = pdev->dev.of_node;		
    ret = of_property_read_u32(pnode, "pwm-gpio", &gpio_nr);	
    if (ret) {	
        pr_err("---+++ get pwm-gpio prop failed\n");	
        return -ENODEV;	
    }	
    if (gpio_nr != 31 && gpio_nr != 32) {	
        pr_err("---+++ pwm-gpio must be 31 or 32\n");	
        return -EINVAL;	
    }
		
    ret = of_property_read_u32(pnode, "pwm-freq", &freq);	
    if (ret)	
        pr_err("---+++ get pwm-freq prop failed\n");	
	
    ret = of_property_read_u32(pnode, "pwm-duty", &duty);	
    if (ret)	
        pr_err("---+++ get pwm-duty prop failed\n");
		
    base_clk  = ioremap(0x1800000, 0x80000);	
    base_gpio = ioremap(0x1000000, 0x300000);
		
    calculate_MND_reg_addr();	
    pwm_reg_save_default();	
    if(freq > 0 && duty > 0 && duty < 100)	
        pwm_output(freq, duty);   
	
    gpio_clk_proc_entry = proc_create(GPIO_PWM_CLOCK, 0666, NULL, &gpio_clk_proc_ops);		
    pr_err("---+++ %s succeeded\n", __func__);
		
    return 0;	
}
	
	
static int gpio_clk_remove (struct platform_device *pdev)	
{	
    remove_proc_entry(GPIO_PWM_CLOCK, NULL);	
    pwm_reg_restore_default();	
    iounmap(base_clk);	
    iounmap(base_gpio);	
	
    return 0;	
}
		
static int gpio_clk_suspend (struct device *dev)	
{	
    if (suspend_flag)	
        return 0;
		
    suspend_flag = 1;	
    pwm_reg_restore_default();	
	
    return 0;	
}
		
static int gpio_clk_resume (struct device *dev)	
{	
    if (!suspend_flag)	
        return 0;	
	
    if (pwm_enable)	
        update_pwm_reg();	
	
    suspend_flag = 0;
		
    return 0;	
}
		
static struct dev_pm_ops gpio_clk_pm = {	
    .suspend = gpio_clk_suspend,	
    .resume  = gpio_clk_resume,	
};
		
static struct of_device_id gpio_clk_match_table[] = {	
    {.compatible = "qcom,gpio_clk",},	
    { },	
};
		
static struct platform_driver gpio_clk_driver = {	
    .probe    = gpio_clk_probe,	
    .remove   = gpio_clk_remove,	
    .driver   = {	
        .name           = "gpio_clk",	
        .owner          = THIS_MODULE,	
        .pm             = &gpio_clk_pm,	
        .of_match_table = gpio_clk_match_table,	
    },
	
};
		
static int __init gpio_pwm_init(void)	
{	
	return platform_driver_register(&gpio_clk_driver);	
}
		
static void __exit gpio_pwm_exit(void)	
{	
	return platform_driver_unregister(&gpio_clk_driver);	
}
	
//module_platform_driver(gpio_clk_driver);
late_initcall(gpio_pwm_init);	
module_exit(gpio_pwm_exit);	
MODULE_DESCRIPTION("GPIO output PWM Driver");	
MODULE_LICENSE("GPL");