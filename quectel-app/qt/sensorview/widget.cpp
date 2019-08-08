#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "widget.h"
#include "ui_widget.h"
#include "ql_sensor.h"

static QString accel_x;
static QString accel_y;
static QString accel_z;
static QString gyro_x;
static QString gyro_y;
static QString gyro_z;
static QString mag_x;
static QString mag_y;
static QString mag_z;
static QString light_l;
static QString prox_d;

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	accel_sensor = new Sensor("accel");
	gyro_sensor  = new Sensor("gyro");
	light_sensor = new Sensor("light");
	prox_sensor  = new Sensor("prox");
	mag_sensor   = new Sensor("mag");

	connect(accel_sensor, SIGNAL(send_accel_data()), this, SLOT(print_accel_data()));
	connect(gyro_sensor,  SIGNAL(send_gyro_data()), this, SLOT(print_gyro_data()));
	connect(light_sensor, SIGNAL(send_light_data()), this, SLOT(print_light_data()));
	connect(prox_sensor,  SIGNAL(send_prox_data()), this, SLOT(print_prox_data()));
	connect(mag_sensor,   SIGNAL(send_mag_data()), this, SLOT(print_mag_data()));

	accel_sensor->start();
	gyro_sensor->start();
	light_sensor->start();
	prox_sensor->start();
	mag_sensor->start();

	accel_sensor->enable("accel", 1);
	gyro_sensor->enable("gyro", 1);
	light_sensor->enable("mag", 1);
	prox_sensor->enable("light", 1);
	mag_sensor->enable("prox", 1);
}

void Widget::print_accel_data(void)
{
	ui->label_acc_x->setText(accel_x);
	ui->label_acc_y->setText(accel_y);
	ui->label_acc_z->setText(accel_z);
}

void Widget::print_gyro_data(void)
{
	ui->label_gyo_x->setText(gyro_x);
	ui->label_gyo_y->setText(gyro_y);
	ui->label_gyo_z->setText(gyro_z);
}

void Widget::print_light_data(void)
{
	
	ui->label_lig_x->setText(light_l);
}

void Widget::print_prox_data(void)
{
	ui->label_pro_x->setText(prox_d);
}

void Widget::print_mag_data(void)
{
	ui->label_mag_x->setText(mag_x);
	ui->label_mag_y->setText(mag_y);
	ui->label_mag_z->setText(mag_z);
}

Sensor::Sensor(const char *type)
{
	sensor_type = type;
}

void Sensor::run()
{
	if (!strcmp(sensor_type, "accel"))
		accel_read_thread();
	else if (!strcmp(sensor_type, "gyro"))
		gyro_read_thread();
	else if (!strcmp(sensor_type, "light"))
		light_read_thread();
	else if (!strcmp(sensor_type, "prox"))
		prox_read_thread();
	else if (!strcmp(sensor_type, "mag"))
		mag_read_thread();
}

void Sensor::accel_read_thread(void)
{
	float value;
	int i, fd, count;
	struct input_event *event;
	struct input_event events[MAX_INPUT_EVENTS];

	fd = open(ACCEL_EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s failed\n", ACCEL_EVENT_FILE_NAME);
		return;
	}
	while (1) {
		count = read(fd, events, MAX_INPUT_EVENTS * sizeof(struct input_event));
		if (count < 0) {
			fprintf(stderr, "read data from %s failed\n", ACCEL_EVENT_FILE_NAME);
			return;
		}
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (event->type == EV_SYN)
				continue;

		value = event->value * (GRAVITY_EARTH / 16384);
		if (event->code == ABS_X)
			accel_x.sprintf("%f", value);
		if (event->code == ABS_Y)
			accel_y.sprintf("%f", value);
		if (event->code == ABS_Z)
			accel_z.sprintf("%f", value);
		emit this->send_accel_data();
		}
	}
	close(fd);
}

void Sensor::gyro_read_thread(void)
{
	float value;
	int i, fd, count;
	struct input_event *event;
	struct input_event events[MAX_INPUT_EVENTS];

	fd = open(GYRO_EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s failed\n", GYRO_EVENT_FILE_NAME);
		return;
	}
	while (1) {
		count = read(fd, events, MAX_INPUT_EVENTS * sizeof(struct input_event));
		if (count < 0) {
			fprintf(stderr, "read data from %s failed\n", GYRO_EVENT_FILE_NAME);
			return;
		}
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (event->type == EV_SYN)
				continue;

			value = event->value * GYROSCOPE_CONVERT * -1;
			if (event->code == ABS_RX)
				gyro_x.sprintf("%f", value);
			if (event->code == ABS_RY)
				gyro_y.sprintf("%f", value);
			if (event->code == ABS_RZ)
				gyro_z.sprintf("%f", value);
			emit this->send_gyro_data();
		}
	}
	close(fd);
}

void Sensor::light_read_thread(void)
{
	int i, fd, count;
	struct input_event *event;
	struct input_event events[MAX_INPUT_EVENTS];

	fd = open(LIGHT_EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s failed\n", LIGHT_EVENT_FILE_NAME);
		return;
	}
	while (1) {
		count = read(fd, events, MAX_INPUT_EVENTS * sizeof(struct input_event));
		if (count < 0) {
			fprintf(stderr, "read data from %s failed\n", LIGHT_EVENT_FILE_NAME);
			return;
		}
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (event->type == EV_SYN)
				continue;

			light_l.sprintf("%ld",  event->value);
			emit this->send_light_data();
		}
	}
	close(fd);
}

void Sensor::prox_read_thread(void)
{
	int i, fd, count;
	struct input_event *event;
	struct input_event events[MAX_INPUT_EVENTS];

	fd = open(PROX_EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s failed\n", PROX_EVENT_FILE_NAME);
		return;
	}
	while (1) {
		count = read(fd, events, MAX_INPUT_EVENTS * sizeof(struct input_event));
		if (count < 0) {
			fprintf(stderr, "read data from %s failed\n", PROX_EVENT_FILE_NAME);
			return;
		}
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (event->type == EV_SYN)
				continue;

			prox_d.sprintf("%s", event->value ? "Far" : "Near");
			emit this->send_prox_data();
		}
	}
	close(fd);
}

void Sensor::mag_read_thread(void)
{
	float value;
	int i, fd, count;
	struct input_event *event;
	struct input_event events[MAX_INPUT_EVENTS];

	fd = open(MAG_EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s failed\n", MAG_EVENT_FILE_NAME);
		return;
	}
	while (1) {
		count = read(fd, events, MAX_INPUT_EVENTS * sizeof(struct input_event));
		if (count < 0) {
			fprintf(stderr, "read data from %s failed\n", MAG_EVENT_FILE_NAME);
			return;
		}
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (event->type == EV_SYN)
				continue;

			value = event->value * CONVERT_MAG;
			if (event->code == ABS_X)
				mag_x.sprintf("%f", value);
			if (event->code == ABS_Y)
				mag_y.sprintf("%f", value);
			if (event->code == ABS_Z)
				mag_z.sprintf("%f", value);
			emit this->send_mag_data();
		}
	}
	close(fd);
}

int Sensor::enable(const char *name, int enable)
{
	int ret, fd;
	char buf[1];
	char enable_file_name[64];

	memset(enable_file_name, 0, sizeof(enable_file_name));
	if (!strcmp(name, "accel"))
		strcpy(enable_file_name, ACCEL_ENABLE_FILE_NAME);
	else if (!strcmp(name, "gyro"))
		strcpy(enable_file_name, GYRO_ENABLE_FILE_NAME);
	else if (!strcmp(name, "light"))
		strcpy(enable_file_name, LIGHT_ENABLE_FILE_NAME);
	else if (!strcmp(name, "prox"))
		strcpy(enable_file_name, PROX_ENABLE_FILE_NAME);
	else if (!strcmp(name, "mag"))
		strcpy(enable_file_name, MAG_ENABLE_FILE_NAME);
	else {
		fprintf(stderr, "invalid sensor name [%s]\n", name);
		return -2;
	}

	fd = open(enable_file_name, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open %s failed\n", name);
		return -3;
	}
	buf[0] = enable ? '1' : '0';
	ret = write(fd, buf, sizeof(buf));
	if (ret != 1) {
		fprintf(stderr, "write %s failed\n", name);
		return -4;
	}
	close(fd);

	return 0;
}

Widget::~Widget()
{
	delete ui;
}

void Widget::on_pushButton_Exit_clicked()
{
	accel_sensor->enable("accel", 0);
	gyro_sensor->enable("gyro", 0);
	light_sensor->enable("mag", 0);
	prox_sensor->enable("light", 0);
	mag_sensor->enable("prox", 0);
	
	delete accel_sensor;
	delete gyro_sensor;
	delete light_sensor;
	delete prox_sensor;
	delete mag_sensor;
	close();
}
