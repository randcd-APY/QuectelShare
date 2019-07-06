#include "gallery.h"
#include "ui_gallery.h"

#include "QMenu"
#include "QMenuBar"
#include "QAction"
#include "QMessageBox"
#include "QFileDialog"
#include "QDebug"
#include "QListWidget"
#include "QPixmap"
#include "QLabel"
#include "filelist.h"
#include <QDesktopWidget>

GETPPATH *newGetPath = NULL;

Gallery::Gallery(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gallery)
{
    ui->setupUi(this);
    newGetPath = new GETPPATH;
	getScreenInfo();
    QString filePath = newGetPath->getPicPath();
    fileOpenActionSlot(filePath);


}

Gallery::~Gallery()
{
    delete ui;
}


void Gallery::getScreenInfo()  //得到当前设备的屏幕分辨率
{
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    ScreenW = screenRect.width();
    ScreenH = screenRect.height();
}

void Gallery::fileOpenActionSlot(QString fileName)
{
//    QIcon icon;
//    icon.addFile(tr(":/back.jpg"));
//    ui->BackButton->setIcon(icon);
//    ui->BackButton->resize(80,80);
    ui->BackButton->setGeometry(50, 1200, 80, 80);
    //ui->BackButton->setIconSize(80,80);
    ui->BackButton->setText("");
    ui->BackButton->setStyleSheet("border-image:url(:/back.jpg)");
    selectFile(fileName);
}
/****************************************
* Qt中使用文件选择对话框步骤如下：
* 1. 定义一个QFileDialog对象
* 2. 设置路径、过滤器等属性
*****************************************/
void Gallery::selectFile(QString fileName)
{

    //QString fileName = QFileDialog::getOpenFileName(this,"Choose Image","Y:\\work\\SC20_Android8.1_R07_r00043_linux\\",("Image File(*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty()){
        this->close();
    }
    QImage image;
    image.load(fileName);
#if 0
    ui->photoShow->setPixmap(QPixmap::fromImage(image));
    //label->setScaledContents(true);
    //label->resize(this->size());
    //ui->photoShow->resize(QSize(image.width(),image.height()));
    //ui->photoShow->setGeometry((720-image.width())/2,(1280-image.height())/2,image.width(),image.height());
    ui->photoShow->show();
#endif
#if 0
    //int imgWidth=image.width();
    //int imgHeight=image.height();
	//if(imgWidth !> ScreenW && imgHeight !> ScreenH)
	//	ui->photoShow->setGeometry((ScreenW-image.width())/2,(ScreenH-image.height())/2,image.width(),image.height());
	QPixmap *pixmap = new QPixmap(fileName);
	ui->photoShow->setGeometry(0,0,ScreenW, ScreenH);
	QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	//pixmap->scaled(ui->photoShow->size(), Qt::KeepAspectRatio);
	ui->photoShow->setScaledContents(true);
	ui->photoShow->setPixmap(*pixmap);
#endif
#if 1
	ui->photoShow->setGeometry(0,0,ScreenW, ScreenH);
    int imgWidth=image.width();
    int imgHeight=image.height();
    int labelWidth=ui->photoShow->width();
    int labelHeight=ui->photoShow->height();
    double widthRate=(double)labelWidth/(double)imgWidth;
    double heightRate=(double)labelHeight/(double)imgHeight;
    if(widthRate<=heightRate)
    {
        QPixmap *qimg=new QPixmap(fileName);
        *qimg = qimg->scaled(QSize(labelWidth,(int)imgHeight*heightRate), Qt::KeepAspectRatio);
        ui->photoShow->setAlignment(Qt::AlignCenter);
        ui->photoShow->setPixmap(*qimg);
    }
    else
    {
        QPixmap *qimg=new QPixmap(fileName);
        *qimg = qimg->scaled(QSize((int)imgWidth*widthRate,labelHeight), Qt::KeepAspectRatio);
        ui->photoShow->setAlignment(Qt::AlignCenter);
        ui->photoShow->setPixmap(*qimg);
	}
#endif

}


void Gallery::on_BackButton_clicked()
{
    this->close();
    return;
    //返回即可

    ui->photoShow->close();
    ui->BackButton->setEnabled(false);
    QString fileName = QFileDialog::getOpenFileName(this,"Choose Image","Y:\\work\\SC20_Android8.1_R07_r00043_linux\\",("Image File(*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty()){
        this->close();
    }
    QImage image;
    image.load(fileName);
    //QLabel *label = new QLabel;
    ui->photoShow->setPixmap(QPixmap::fromImage(image));
    //label->setScaledContents(true);
    //label->resize(this->size());
    //ui->photoShow->resize(QSize(image.width(),image.height()));
    ui->photoShow->setGeometry((720-image.width())/2,(1280-image.height())/2,image.width(),image.height());
    ui->photoShow->show();
    ui->BackButton->setEnabled(true);

}
