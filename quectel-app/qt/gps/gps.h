#ifndef GPS_H
#define GPS_H

#include <QDialog>
#include <ql_gnss.h>
#include <QThread>


namespace Ui {
class gps;
}

class gps : public QDialog
{
    Q_OBJECT

public:
    explicit gps(QWidget *parent = nullptr);
    ~gps();


private slots:
    void gps_update();
    void on_ButtonStart_clicked();

    void on_ButtonStop_clicked();

    void on_ButtonExit_clicked();

private:
    Ui::gps *ui;
};


class myThread: public QThread
{
public:
    myThread();
    void run();
};
#endif // GPS_H
