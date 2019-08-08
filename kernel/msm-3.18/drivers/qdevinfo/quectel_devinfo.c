/*************************************************************************
  > File Name: quectel_devinfo.c
  > Author: geoff.liu
  > Mail: geoff.liu@quectel.com 
  > Created Time: 2018-11-25
  > Describe : add this driver for get device information(AT+QDEVINFO)
  >Note : modify code:\kernel\drivers\platform\msm\qpnp-revid.c for get pmic information
 ************************************************************************/

#include <linux/kernel.h>  
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/proc_fs.h>  //必须要包含的头文件
#include <linux/seq_file.h>  //必须要包含的头文件
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define QUECTEL_QDEVINFO_CMD 
#ifdef QUECTEL_QDEVINFO_CMD

#define EMMC_NAME_STR_LEN 32
#define DDR_STR_LEN 64
#define EXT_CSD_STR_LEN 1025
#define MMC_NAME "/sys/class/mmc_host/mmc0/mmc0:0001/name"
#define MMC_EXT_CSD "/sys/kernel/debug/mmc0/mmc0:0001/ext_csd"
#define MEMINFO "/proc/meminfo"

//#define MMC_EXT_CSD "/data/ext_csd"
int get_buf(const char *filename,char *buf,int size) {
    /*read file */
    int length;
    struct file *fp;
    mm_segment_t fs;
    loff_t pos;
    fp =filp_open(filename,O_RDONLY | O_LARGEFILE, 0);  
    if (IS_ERR(fp)){ 
        pr_err("create file error/n");
        return -1;
    } 
    fs =get_fs();
    set_fs(KERNEL_DS);
    pos =0; 
    length=vfs_read(fp,buf, size, &pos);
    filp_close(fp,NULL);
    set_fs(fs);
    return length; 
}

char change_char_hex(char c)
{
    //int result=0;
    if((c>='0')&&(c<='9'))
        return (c-'0');
    else if((c>='a')&&(c<='f'))
        return (c-'a'+10);
    else if((c>='A')&&(c<='F'))
        return (c-'A'+10);
    else return 0;
}

char change_char_excsd(char c1,char c2)
{
    return 	change_char_hex(c1)*16+change_char_hex(c2);
}
static u32 quec_get_emmc_size_mb(void)
{
    char *kbuf;
    char mmc_size[4];//ext_csd[215:212]
    u32 capacity=0;
    kbuf = kmalloc(EXT_CSD_STR_LEN + 1, GFP_KERNEL);
    memset(kbuf,0,EXT_CSD_STR_LEN + 1);
    get_buf(MMC_EXT_CSD,kbuf,EXT_CSD_STR_LEN);
    mmc_size[0]=change_char_excsd(kbuf[424],kbuf[425]);
    mmc_size[1]=change_char_excsd(kbuf[426],kbuf[427]);
    mmc_size[2]=change_char_excsd(kbuf[428],kbuf[429]);
    mmc_size[3]=change_char_excsd(kbuf[430],kbuf[431]);
    memcpy(&capacity,mmc_size,4);
    //seq_printf(m, "%uM\n",capacity/2048);
    kfree(kbuf);
    return capacity/2048;
}
static int quec_get_emmc_size_gb(void)
{

    u32 capacity_mb=0;
    int size_gb;
    capacity_mb=quec_get_emmc_size_mb();
    if(capacity_mb%4096==0)
        size_gb = (capacity_mb/4096)*4;
    else
        size_gb = (capacity_mb/4096+1)*4;
    return size_gb;
}
static int quec_get_ddr_size_mb(void)
{
    char *kbuf;
    int i,result=0;;
    kbuf = kmalloc(DDR_STR_LEN, GFP_KERNEL);
    memset(kbuf,0,DDR_STR_LEN);
    get_buf(MEMINFO,kbuf,DDR_STR_LEN);
    for(i=0;kbuf[i]!='\n';i++)
    {
        if((kbuf[i]>'/')&&(kbuf[i]<':'))
            result=result*10+kbuf[i]-'0';
    }
    //seq_printf(m, "%uM\n",capacity/2048);
    kfree(kbuf);
    return result/1024;
}

static int quec_emmc_ext_csd_proc_show(struct seq_file *m, void *v)
{
    char *kbuf;
    kbuf = kmalloc(EXT_CSD_STR_LEN + 1, GFP_KERNEL);
    memset(kbuf,0,EXT_CSD_STR_LEN + 1);
    get_buf(MMC_EXT_CSD,kbuf,EXT_CSD_STR_LEN);
    seq_printf(m, "%s",kbuf);
    kfree(kbuf);
    return 0;
}
//struct proc_dir_entry *entry = NULL;
static int quec_emmc_ext_csd_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_emmc_ext_csd_proc_show,inode->i_private);  
}

static const struct file_operations quec_emmc_ext_csd_proc_fops = {
    .open		= quec_emmc_ext_csd_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};

static int quec_emmc_size_proc_show(struct seq_file *m, void *v)
{

    u32 capacity=0;
    capacity=quec_get_emmc_size_mb();
    seq_printf(m, "%uM\n",capacity);
    return 0;
}
//struct proc_dir_entry *entry = NULL;
static int quec_emmc_size_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_emmc_size_proc_show,inode->i_private);  
}

static const struct file_operations quec_emmc_size_proc_fops = {
    .open		= quec_emmc_size_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};


static int quec_emmc_eol_proc_show(struct seq_file *m, void *v)
{

    char *kbuf;
    char mmc_eol;//ext_csd[267]
    kbuf = kmalloc(EXT_CSD_STR_LEN + 1, GFP_KERNEL);
    memset(kbuf,0,EXT_CSD_STR_LEN + 1);
    get_buf(MMC_EXT_CSD,kbuf,EXT_CSD_STR_LEN);
    mmc_eol=change_char_excsd(kbuf[534],kbuf[535]);
    seq_printf(m, "emmc_eol[%04d]\n",mmc_eol);
    kfree(kbuf);
    return 0;
}
//struct proc_dir_entry *entry = NULL;
static int quec_emmc_eol_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_emmc_eol_proc_show,inode->i_private);  
}

static const struct file_operations quec_emmc_eol_proc_fops = {
    .open		= quec_emmc_eol_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};


static int quec_emmc_life_proc_show(struct seq_file *m, void *v)
{

    char *kbuf;
    char mmc_life[2];//ext_csd[268:269]
    kbuf = kmalloc(EXT_CSD_STR_LEN + 1, GFP_KERNEL);
    memset(kbuf,0,EXT_CSD_STR_LEN + 1);
    get_buf(MMC_EXT_CSD,kbuf,EXT_CSD_STR_LEN);
    mmc_life[0]=change_char_excsd(kbuf[536],kbuf[537]);
    mmc_life[1]=change_char_excsd(kbuf[538],kbuf[539]);
    seq_printf(m, "emmc_life_time[%04x%04x]\n",mmc_life[0],mmc_life[1]);
    kfree(kbuf);
    return 0;
}
static int quec_emmc_life_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_emmc_life_proc_show,inode->i_private);  
}

static const struct file_operations quec_emmc_life_proc_fops = {
    .open		= quec_emmc_life_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};

static int quec_emmc_health_proc_show(struct seq_file *m, void *v)
{

    char *kbuf;
    int i,j;
    char mmc_health;//ext_csd factory[270:285] ext_csd runtime[286:301]
    kbuf = kmalloc(EXT_CSD_STR_LEN + 1, GFP_KERNEL);
    memset(kbuf,0,EXT_CSD_STR_LEN + 1);
    get_buf(MMC_EXT_CSD,kbuf,EXT_CSD_STR_LEN);
    seq_printf(m, "mmc_health_factory[");
    for(i=540;i<572;i++)
    {
        j=i+1;
        mmc_health=change_char_excsd(kbuf[i],kbuf[j]);
        seq_printf(m,"%2x",mmc_health);
        i++;
    }
    seq_printf(m, "]\n");
    seq_printf(m, "mmc_health_runtime[");
    for(i=572;i<604;i++)
    {
        j=i+1;
        mmc_health=change_char_excsd(kbuf[i],kbuf[j]);
        seq_printf(m, "%02x",mmc_health);
        i++;
    }
    seq_printf(m, "]\n");
    kfree(kbuf);
    return 0;
}

static int quec_emmc_health_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_emmc_health_proc_show,inode->i_private);  
}

static const struct file_operations quec_emmc_health_proc_fops = {
    .open		= quec_emmc_health_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};

static int quec_emcp_info_proc_show(struct seq_file *m, void *v)
{

    char *kbuf;
    int ddr_size=0;
    int i=0;
    kbuf = kmalloc(EMMC_NAME_STR_LEN , GFP_KERNEL);
    memset(kbuf,0,EMMC_NAME_STR_LEN);
    ddr_size=quec_get_ddr_size_mb();
    get_buf(MMC_NAME,kbuf,EMMC_NAME_STR_LEN);
    for(i=0;i<EMMC_NAME_STR_LEN;i++)
    {
        if(kbuf[i]=='\n')
        {
            kbuf[i]='\0';
            break;
        }
    }
    if(ddr_size<=512)
        seq_printf(m, "%s,%dG,512M\n",kbuf,quec_get_emmc_size_gb());
    else
    {
        ddr_size=ddr_size/1024+1;
        seq_printf(m, "%s,%dG,%dG\n",kbuf,quec_get_emmc_size_gb(),ddr_size);
    }	
    kfree(kbuf);
    return 0;
}

static int quec_emcp_info_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_emcp_info_proc_show,inode->i_private);  
}

static const struct file_operations quec_emcp_info_proc_fops = {
    .open		= quec_emcp_info_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};
extern void quectel_get_pmic_info(char *buf);
static int quec_pmu_info_proc_show(struct seq_file *m, void *v)
{

    char pmu_info[64] = {'\0'};
    quectel_get_pmic_info(pmu_info);
    seq_printf(m, "%s\n",pmu_info);
    return 0;
}

static int quec_pmu_info_proc_open(struct inode *inode, struct file *filp)
{
    return single_open(filp, quec_pmu_info_proc_show,inode->i_private);  
}

static const struct file_operations quec_pmu_info_proc_fops = {
    .open		= quec_pmu_info_proc_open, 
    .read		= seq_read,  
    .llseek		= seq_lseek, 
    .release	= single_release, 
};


static int qdevinfo_proc_create(void)
{
    proc_create("quec_emmc_ext_csd", 0444, NULL, &quec_emmc_ext_csd_proc_fops);
    proc_create("quec_emmc_size", 0444, NULL, &quec_emmc_size_proc_fops);
    proc_create("quec_emmc_eol", 0444, NULL, &quec_emmc_eol_proc_fops);
    proc_create("quec_emmc_life", 0444, NULL, &quec_emmc_life_proc_fops);
    proc_create("quec_emmc_health", 0444, NULL, &quec_emmc_health_proc_fops);
    proc_create("quec_emcp_info", 0444, NULL, &quec_emcp_info_proc_fops);
    //proc_create("quec_bb_info", 0444, NULL, &quec_emmc_ext_csd_proc_fops);
    proc_create("quec_pmu_info", 0444, NULL, &quec_pmu_info_proc_fops);
    return 0;
}
#endif

static int __init quec_devinfo_init(void)
{
    if(qdevinfo_proc_create())
        pr_err("proc create failed\n");
    return 0;
}

static void __exit quec_devinfo_exit(void)
{
    pr_err("quec devinfo Exit!\n");
}

module_init(quec_devinfo_init);
module_exit(quec_devinfo_exit);
MODULE_AUTHOR("geoff.liu_quectel");  
MODULE_LICENSE("GPL");
