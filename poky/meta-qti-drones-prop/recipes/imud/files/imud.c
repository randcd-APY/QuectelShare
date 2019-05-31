/***************************************************************************//**
@copyright
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

#define CLI_FD_NUM 1

#define IDLE 0
#define INIT 1
#define INIT_RAW 11
#define START 2
#define STOP 3

#define MMAP_SIZE 4096
#define DATA_LENGTH 24

#define CONFIG_FILE "/data/imud.conf"

#define DEV "/sys/devices/platform/soc/78b5000.i2c/i2c-1/1-0068/iio:device0"
#define BUF "/sys/devices/platform/soc/78b5000.i2c/i2c-1/1-0068/iio:device0/buffer"
#define ELEMENT "/sys/devices/platform/soc/78b5000.i2c/i2c-1/1-0068/iio:device0/scan_elements"

#define GYRO_FSR 1000
#define ACC_FSR 4
#define full_range 32768
#define PI 3.14159
#define combine_8_to_16(upper, lower) ((upper << 8) | lower)

char *chanel_table[8][3] =
{
	ELEMENT,"in_accel_x_en","1",
	ELEMENT,"in_accel_y_en","1",
	ELEMENT,"in_accel_z_en","1",
	ELEMENT,"in_anglvel_x_en","1",
	ELEMENT,"in_anglvel_y_en","1",
	ELEMENT,"in_anglvel_z_en","1",
	ELEMENT,"in_temp_en","1",
	ELEMENT,"in_timestamp_en","1",
};

char *dev_table[3][3] =
{
	DEV,"inv_icm20602_init","1",
	DEV,"inv_user_fps_in_ms","20",
	DEV,"inv_icm20602_init","1",
};

char *buf_table[2][3] =
{
	BUF,"length","1024",
	BUF,"enable","1",
};

struct struct_icm20602_raw_data {
	uint8_t ACCEL_XOUT_H;
	uint8_t ACCEL_XOUT_L;

	uint8_t ACCEL_YOUT_H;
	uint8_t ACCEL_YOUT_L;

	uint8_t ACCEL_ZOUT_H;
	uint8_t ACCEL_ZOUT_L;

	uint8_t TEMP_OUT_H;
	uint8_t TEMP_OUT_L;

	uint8_t GYRO_XOUT_H;
	uint8_t GYRO_XOUT_L;

	uint8_t GYRO_YOUT_H;
	uint8_t GYRO_YOUT_L;

	uint8_t GYRO_ZOUT_H;
	uint8_t GYRO_ZOUT_L;
};

struct imu_pack{
	double angular_velocity_x;
	double angular_velocity_y;
	double angular_velocity_z;
	double acceloration_x;
	double acceloration_y;
	double acceloration_z;
	int16_t temperature;
	uint64_t time;
};

struct imu_raw{
	int16_t gyro_raw_x;
	int16_t gyro_raw_y;
	int16_t gyro_raw_z;
	int16_t acc_raw_x;
	int16_t acc_raw_y;
	int16_t acc_raw_z;
	int16_t temperature;
	uint64_t time;
};

struct imu_clinet{
	int id;
	int running;
};

static int debug = 0;
static int run = 0;
static int over = 0;
static int raw = 0;
static int mmap_fd = 0;
static int fifo = 0;
static char *map = NULL;
static int iio_fp;
static struct imu_pack ip={0};
static struct imu_raw ir={0};

void init_mmap(void)
{
	printf("imud : init_mmap\n");

	mmap_fd = open("/data/imu_map", O_CREAT|O_RDWR|O_TRUNC, 00644);
	ftruncate(mmap_fd, MMAP_SIZE);
	printf("imud :  mmap_fd = %d\n", mmap_fd);
	if(mmap_fd > 0)
	{
		map = (char *)mmap(0, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mmap_fd, 0);
		if(map)
		{
			printf("imud : init_mmap : map addr = %p\n", map);
			memset(map, 0, MMAP_SIZE);
			printf("imud : init_mmap : share_mem set\n");
		}
		else
		{
			printf("imud : init_mmap : map failed\n");
		}
	}
	else
		printf("imud : init_mmap : open failed\n");
}

int parse_config(void)
{
	char buf[256] = {0};
	int flen=0, i = 0;

	FILE *fp = NULL;

	fp = fopen(CONFIG_FILE, "r");
	if(fp == NULL)
	{
		printf("imud: parse_config: config file open error");
		fclose(fp);
		return 1;
	}

	fseek(fp,0L,SEEK_END);
	flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	printf("imud: parse_config: config size:%d\n", flen);

	fread(buf, 1, flen, fp);

	/*
	TODO : will use this code when more config is required
	sscanf(buf, "%*[^:]:%s%*[^:]:%s%*[^:]:%s%*[^:]:%s%*[^:]:%s%*[^:]:%s%*[^:]:%s%*[^:]:%s\
		%*[^:]:%s\
		%*[^:]:%s%*[^:]:%s",
		chanel_table[0][2], chanel_table[1][2],chanel_table[2][2],chanel_table[3][2],chanel_table[4][2],chanel_table[5][2],chanel_table[6][2],chanel_table[7][2],
		dev_table[1][2],
		buf_table[0][2], buf_table[1][2]);
	*/
	sscanf(buf, "%*[^:]:%s", dev_table[1][2]);

	printf("imud: parse_config: done!");
	fclose(fp);

	return 0;
}

static void set_config(void)
{
	int i;
	FILE *cfp = NULL;
	char temp[256] = {0};

	printf("setting chanel\n");
	for(i=0; i<8; i++)
	{
		memset(temp, 0, 256);
		sprintf(temp, "%s/%s", chanel_table[i][0], chanel_table[i][1]);
		printf("chanel path : %s\n", temp);
		cfp = fopen(temp, "w");
		fprintf(cfp, "%s", chanel_table[i][2]);
		fclose(cfp);
		cfp = NULL;
	}

	printf("setting fps\n");
	for(i=0; i<3; i++)
	{
		memset(temp, 0, 256);
		sprintf(temp, "%s/%s", dev_table[i][0], dev_table[i][1]);
		printf("dev path : %s\n", temp);
		cfp = fopen(temp, "w");
		fprintf(cfp, "%s", dev_table[i][2]);
		fclose(cfp);
		cfp = NULL;
	}

	printf("setting buf\n");
	for(i=0; i<2; i++)
	{
		memset(temp, 0, 256);
		sprintf(temp, "%s/%s", buf_table[i][0], buf_table[i][1]);
		printf("buf path : %s\n", temp);
		cfp = fopen(temp, "w");
		fprintf(cfp, "%s", buf_table[i][2]);
		fclose(cfp);
		cfp = NULL;
	}
}

static void verify_config(void)
{
	int i;
	int val;
	int ret;
	char temp[128] = {0};

	FILE *cfp=NULL;

	//TODO : cat device node value is not yet finished in driver
	for(i=0; i<1; i++)
	{
		memset(temp, 0, 128);
		sprintf(temp, "%s/%s", dev_table[1][0], dev_table[1][1]);
		cfp = fopen(temp, "r");
		ret = fscanf(cfp, "%d", &val);
		//printf("imud : verify_config : fps : [%d]\n", val);
		fclose(cfp);
	}
}

void parse_data(char *data)
{
	uint32_t gryo_fsr = GYRO_FSR;
	uint32_t acc_fsr = ACC_FSR;
	double gryo_sen = 32768/gryo_fsr;
	double acc_sen = 32768/acc_fsr;

	int16_t gyro_raw_x, gyro_raw_y , gyro_raw_z;
	int16_t acc_raw_x, acc_raw_y , acc_raw_z;
	int16_t temperature;
	struct struct_icm20602_raw_data raw_data;

	memcpy(&raw_data, data, sizeof(struct struct_icm20602_raw_data));

	gyro_raw_x = combine_8_to_16(raw_data.GYRO_XOUT_H, raw_data.GYRO_XOUT_L);
	gyro_raw_y = combine_8_to_16(raw_data.GYRO_YOUT_H, raw_data.GYRO_YOUT_L);
	gyro_raw_z = combine_8_to_16(raw_data.GYRO_ZOUT_H, raw_data.GYRO_ZOUT_L);

	ip.angular_velocity_x = gyro_raw_x / gryo_sen * PI / 180.0;
	ip.angular_velocity_y = gyro_raw_y / gryo_sen * PI / 180.0;
	ip.angular_velocity_z = gyro_raw_z / gryo_sen * PI / 180.0;

	acc_raw_x = combine_8_to_16(raw_data.ACCEL_XOUT_H, raw_data.ACCEL_XOUT_L);
	acc_raw_y = combine_8_to_16(raw_data.ACCEL_YOUT_H, raw_data.ACCEL_YOUT_L);
	acc_raw_z = combine_8_to_16(raw_data.ACCEL_ZOUT_H, raw_data.ACCEL_ZOUT_L);

	ip.acceloration_x = acc_raw_x / acc_sen;
	ip.acceloration_y = acc_raw_y / acc_sen;
	ip.acceloration_z = acc_raw_z / acc_sen;

	temperature = combine_8_to_16(raw_data.TEMP_OUT_H, raw_data.TEMP_OUT_L);

	ip.temperature = temperature/326 + 25;
}

void pack_raw(char *data)
{
	struct struct_icm20602_raw_data raw_data;

	memcpy(&raw_data, data, sizeof(struct struct_icm20602_raw_data));

	ir.gyro_raw_x = combine_8_to_16(raw_data.GYRO_XOUT_H, raw_data.GYRO_XOUT_L);
	ir.gyro_raw_y = combine_8_to_16(raw_data.GYRO_YOUT_H, raw_data.GYRO_YOUT_L);
	ir.gyro_raw_z = combine_8_to_16(raw_data.GYRO_ZOUT_H, raw_data.GYRO_ZOUT_L);

	ir.acc_raw_x = combine_8_to_16(raw_data.ACCEL_XOUT_H, raw_data.ACCEL_XOUT_L);
	ir.acc_raw_y = combine_8_to_16(raw_data.ACCEL_YOUT_H, raw_data.ACCEL_YOUT_L);
	ir.acc_raw_z = combine_8_to_16(raw_data.ACCEL_ZOUT_H, raw_data.ACCEL_ZOUT_L);

	ir.temperature = combine_8_to_16(raw_data.TEMP_OUT_H, raw_data.TEMP_OUT_L);
}

void *data(void){
	unsigned long num_loops = 2;
	unsigned long timedelay = 1000000;
	unsigned long buf_len = 128;
	uint64_t time = 0;
	uint64_t time_last;
	int bytes;
	int j, i=0, ret = 0;
	int noevents = 0, toread = 0, read_size=0;
	char buf[DATA_LENGTH] = {0};
	char *err_log = NULL;

	FILE *csv = NULL;
	csv = fopen("/data/imu_data.csv","w+");

	printf( "imud : data\n");
	iio_fp = open("/dev/iio:device0", O_RDONLY | O_NONBLOCK);
	if (iio_fp == -1) {
		printf("imud : data : Failed to open %s\n", err_log);
	}
	//prepare IMU polling fd
	struct pollfd pfd = {
		.fd = iio_fp,
		.events = POLLIN,
	};

	while(1){
		if(run == 0)
		{
			usleep(timedelay/5);
			continue;
		}

		if (!noevents) {
			struct pollfd pfd = {
				.fd = iio_fp,
				.events = POLLIN,
			};
			poll(&pfd, 1, -1);
			toread = buf_len;
		} else {
			usleep(timedelay);
			toread = 64;
		}
		//printf("start read\n");
		read_size = read(iio_fp,
				 buf,
				 DATA_LENGTH);
		if (read_size < 0) {
			if (errno == -EAGAIN) {
				printf("nothing available\n");
				continue;
			} else
				break;
		}

		if(raw == 0)
			parse_data(buf);
		else if(raw == 1)
			pack_raw(buf);

		time_last = time;
		memcpy(&time, &buf[DATA_LENGTH-8], 8);
		if(time - time_last > 0) i=0;

		if(raw == 0)
			ip.time = time + ( (i++) * 5000000);
		else if(raw == 1)
			ir.time = time + ( (i++) * 5000000);

		if(debug == 1)
		{
			if(raw == 0)
			{
				printf("ip.time: %lld\n\n", ip.time);

				printf("ip.angular_velocity_x: %f\n", ip.angular_velocity_x);
				printf("ip.angular_velocity_y: %f\n", ip.angular_velocity_y);
				printf("ip.angular_velocity_z: %f\n\n", ip.angular_velocity_z);

				printf("ip.acceloration_x: %f\n", ip.acceloration_x);
				printf("ip.acceloration_y: %f\n", ip.acceloration_y);
				printf("ip.acceloration_z: %f\n\n", ip.acceloration_z);

				printf("ip.temperature: %d\n", ip.temperature );
				printf("ip.size: %d\n", sizeof(ip) );
				printf("i : %d\n", i );

				printf("imud: data_thread: timestemp: %lld\n\n", time - time_last);

				fprintf(csv, "\'%lld,%f,%f,%f,%f,%f,%f,%d\n",
					ip.time ,
					ip.angular_velocity_x, ip.angular_velocity_y, ip.angular_velocity_z,
					ip.acceloration_x, ip.acceloration_y, ip.acceloration_z,
					ip.temperature);
			}
			else if(raw == 1)
			{
				printf("ir.gyro_raw_x: %d\n", ir.gyro_raw_x);
				printf("ir.gyro_raw_y: %d\n", ir.gyro_raw_y);
				printf("ir.gyro_raw_z: %d\n\n", ir.gyro_raw_z);

				printf("ir.acc_raw_x: %d\n", ir.acc_raw_x);
				printf("ir.acc_raw_x: %d\n", ir.acc_raw_y);
				printf("ir.acc_raw_x: %d\n\n", ir.acc_raw_z);

				printf("ir.temperature: %d\n", ir.temperature );
			}
		}


		memcpy(map, map+sizeof(ip), MMAP_SIZE-sizeof(ip));
		if(raw == 0)
			memcpy(map+MMAP_SIZE-sizeof(ip), &ip, sizeof(ip));
		else if(raw == 1)
			memcpy(map+MMAP_SIZE-sizeof(ip), &ir, sizeof(ir));

		//struct timeval tv;
		//gettimeofday(&tv, NULL);
		//int64_t ts = (int64_t)tv.tv_sec*1000 + tv.tv_usec/1000;

		//time_last = time;
		//printf("timestemp: %lld\n\n", time - time_last);
	}
	printf("imud: data: -->close fp\n");
	close(iio_fp);
	printf("imud: data: -->unmap\n");
	munmap(map, MMAP_SIZE);
	printf("imud: data: -->data out\n");
	pthread_exit(NULL);
}

void *cmd(void *arg){
	int *param = (int *)arg;
	int client_sockfd = *param;
	int len;
	char ch_recv = 0;

	printf( "imud : cmd[%d]\n", client_sockfd );

	while(1){
		int len = recv(client_sockfd,&ch_recv,1,0);
		if(len > 0)
		{
			if(ch_recv == INIT)
			{
				printf("imud : cmd : recv cmd INIT : %d\n", ch_recv);
				raw = 0;
			}
			else if(ch_recv == START)
			{
				printf("imud : cmd : recv cmd START : %d\n", ch_recv);
				run = 1;
			}
			else if(ch_recv == STOP)
			{
				printf("imud : cmd : recv cmd STOP : %d\n", ch_recv);
				run = 0;
			}
			else if(ch_recv == INIT_RAW)
			{
				printf("imud : cmd : recv cmd RAW_INIT : %d\n", ch_recv);
				raw = 1;
			}
			else
			{
				printf("imud : cmd : not recognized! : %d\n",ch_recv);
			}
		}
		else if(len == 0)
		{
			printf("imud : cmd : len(%d) socket lost\n",len);
			run = 0;
			return NULL;
		}
		else
		{
			printf("imud : cmd : recv error\n");
			run = 0;
			return NULL;
		}
	}
	pthread_exit(NULL);
}

void *unit_test(void *arg){
	int ch_recv = 0;
	printf("imud : unit_test mode\n");

	while(1)
	{
		printf("unit_test command : ");
		scanf("%d", &ch_recv);

		if(ch_recv == INIT)
		{
			printf("imud : unit_test : recv cmd INIT : %d\n", ch_recv);
			raw = 0;
		}
		else if(ch_recv == START)
		{
			printf("imud : unit_test : recv cmd START : %d\n", ch_recv);
			run = 1;
		}
		else if(ch_recv == STOP)
		{
			printf("imud : unit_test : recv cmd STOP : %d\n", ch_recv);
			run = 0;
		}
		else if(ch_recv == INIT_RAW)
		{
			printf("imud :unit_test : recv cmd RAW_INIT : %d\n", ch_recv);
			raw = 1;
		}
	}
	pthread_exit(NULL);
}

void init_imu(pthread_t *tid)
{
	int rc = 0;
	init_mmap();
	set_config();
	rc = pthread_create(tid, NULL, data, NULL);
	if (rc) {
		printf("imud : create data thread failed\n");
	}
	return ;
}

int main (int argc, char *argv[]){
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_un server_address;
	struct sockaddr_un client_address;
	int i = 0, rc = 0;
	pthread_t tid;

	init_imu(&tid);

	if(argc > 1)
	{
		debug = 1;
		pthread_t tid;
		rc = pthread_create(&tid, NULL, unit_test, NULL);
		if (rc) {
			printf("imud : Couldn't create cmd thread \n");
			return 1;
		}
		while(1)
		{
			printf("IMUD UNIT TEST on!\n");
			sleep(5);
		}
	}

	unlink ("/data/imud_socket");

	server_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);

	server_address.sun_family = AF_UNIX;

	strcpy (server_address.sun_path, "/data/imud_socket");

	server_len = sizeof (server_address);

	bind (server_sockfd, (struct sockaddr *)&server_address, server_len);

	listen (server_sockfd, CLI_FD_NUM);
	printf ("imud : Server is waiting for client connect...\n");
	client_len = sizeof (client_address);

	while(1){
		client_sockfd = accept (server_sockfd, (struct sockaddr *)&server_address, (socklen_t*)&client_len);
		if (client_sockfd == -1) {
				perror ("accept");
				break;
		}
		rc = pthread_create(&tid, NULL, cmd, (void *)&client_sockfd);
		if (rc) {
			printf("imud : create cmd thread failed\n");
			return 1;
		}
	}
	close (client_sockfd);
	unlink ("server socket");
}

