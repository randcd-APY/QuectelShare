#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>

class Sensor : public QThread
{
    Q_OBJECT

private:
	const char *sensor_type;
public:
    Sensor(const char *sensor_type);
	void accel_read_thread(void);
    void gyro_read_thread(void);
    void light_read_thread(void);
    void prox_read_thread(void);
    void mag_read_thread(void);
	int enable(const char *name, int enable);

protected:
    void run();

signals:
    void send_accel_data(void);
	void send_gyro_data(void);
	void send_light_data(void);
	void send_prox_data(void);
	void send_mag_data(void);
};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
	Sensor *accel_sensor;
	Sensor *gyro_sensor;
	Sensor *light_sensor;
	Sensor *prox_sensor;
	Sensor *mag_sensor;

private slots:
	void print_accel_data(void);
	void print_gyro_data(void);
	void print_mag_data(void);
	void print_light_data(void);
	void print_prox_data(void);
    void on_pushButton_Exit_clicked();
};

#endif // WIDGET_H
