#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qswitchbutton.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include "alphamessagebox.h"
#include <QAbstractButton>
#include <qdebug.h>
#include <ql_bt.h>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->mSwitch,SIGNAL(clicked()),this,SLOT(on_off()));
    connect(ui->mConBt,SIGNAL(clicked()),this,SLOT(sock_con()));
    connect(ui->mListBt,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(OnListWidgetDoubleClicked(QModelIndex)));
    connect(ui->mExit,SIGNAL(clicked()),qApp,SLOT(quit()));
//    connect(ui->mListBt , SIGNAL(clicked(QListViewItem * )) , this , SLOT(show()));
//    connect(ui->mListBt, SIGNAL(clicked()), this, SLOT(OnListWidgetDoubleClicked(const QModelIndex &index)));

    mModel = new QStandardItemModel;
    ui->mListBt->setModel(mModel);
    ui->mListBt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if(QL_HAL_load() < 0){
        printMsg(this,"HAL library load failed!");
    }
    QL_BT_init();
    sleep(1);
    QL_BT_enable();
}


void MainWindow::sock_con(){
     sock_connect();
}

void MainWindow::OnListWidgetDoubleClicked(const QModelIndex &index){
    // QMessageBox点击按钮后右响应的
    QMessageBox msgBox(QMessageBox::Question,"询问","确定要连接该蓝牙吗？",QMessageBox::Yes|QMessageBox::No,this);
    msgBox.setStyleSheet(
            "QPushButton {"
                            "background-color:#89AFDE;"
                            " border-style: outset;"
                            " border-width: 2px;"
                            " border-radius: 10px;"
                            " border-color: beige;"
                            " font: bold 24px;"
                            " min-width: 5em;"
                            " min-height: 2em;"
                            "}"
            "QLabel {font-size: 30px;}"
        );
    int nRet = msgBox.exec();
    // 选择是
    if (QMessageBox::Yes == nRet) {
        if(this->connect_index != NULL){
            this->connect_index(index.row());
        }
    }

    // 选择否
    if (QMessageBox::No == nRet) {
        qDebug("取消");
    }
}


void MainWindow::updateRecvData(string data){
    ui->mRecvData->setText(QString::fromStdString(data));
}

void MainWindow::updateConnState(int state){
    if(state == 0){ 
        ui->mConBt->setText("Connected to the newly paired BLE");
    }else if(state != 0){ 
        ui->mConBt->setText("The newly matched BLE connection failed. Please reconnect");
    } 
}

void MainWindow::update(string data){
    QStandardItem *item = new QStandardItem(QString::fromStdString(data));
    item->setSizeHint(QSize(700, 100));
    mModel->appendRow(item);
}

void MainWindow::set_ble_clear_list(void (*p)()){
    if(NULL != p){
        this->clear_ble_list = p;
    }
}

void MainWindow::set_connect_index(void (*p)(int index)){
    if(NULL != p){
        this->connect_index = p;
    }
}

void MainWindow::set_sock_connect(void (*p)()){
    if(NULL != p){ 
        this->sock_connect = p;
    }   
}


void MainWindow::on_off(){
    if(!ui->mSwitch->IsSelected()){
        QL_BT_scan(); 
    }else {
        mModel->clear();
        QL_BT_unscan();
        if(this->clear_ble_list != NULL){
            this->clear_ble_list();
        }
    }
}

void MainWindow::printMsg(QWidget *parent,QString msg){
    // QMessageBox点击按钮后右响应的
    QMessageBox msgBox(QMessageBox::Question,"询问",msg,QMessageBox::Yes|QMessageBox::No,parent);
    msgBox.setStyleSheet(
            "QPushButton {"
            "background-color:#89AFDE;"
            " border-style: outset;"
            " border-width: 2px;"
            " border-radius: 10px;"
            " border-color: beige;"
            " font: bold 24px;"
                            " min-width: 5em;"
                            " min-height: 2em;"
                            "}"
            "QLabel {font-size: 30px;}"
        );
    int nRet = msgBox.exec();
    // 选择是
    if (QMessageBox::Yes == nRet) {
        qDebug() << "helo";
    }

    // 选择否
    if (QMessageBox::No == nRet) {
        qDebug("取消");
    }

}


MainWindow::~MainWindow()
{
    QL_BT_disable();
    delete ui;
}
