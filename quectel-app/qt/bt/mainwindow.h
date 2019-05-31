#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QStandardItemModel>
#include <QListWidgetItem>

#include <string>

using namespace std;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void printMsg(QWidget *parent,QString msg);
    void update(string data);
    void updateRecvData(string data);
    void updateConnState(int state);
    void set_ble_clear_list(void (*p)());
    void set_connect_index(void (*p)(int index));
    void set_sock_connect(void (*p)());
    ~MainWindow();

public slots:
    void on_off();
    void sock_con();
    void OnListWidgetDoubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *mModel;
    void (*clear_ble_list)();
    void (*connect_index)(int index);
    void (*sock_connect)();
};

#endif // MAINWINDOW_H
