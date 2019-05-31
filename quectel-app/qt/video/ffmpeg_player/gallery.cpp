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
#include <QPainter>

GETPPATH *newGetPath = NULL;
Gallery *pThis;

Gallery::Gallery(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gallery),
	isPlaying(false),
	vidPlayer(new VideoPlayer())
{
    ui->setupUi(this);
	pThis = this;

	vidRect = QRect(0, 360, 720, 480);
    newGetPath = new GETPPATH;
    QString filePath = newGetPath->getVidPath();
    fileOpenActionSlot(filePath);

}

Gallery::~Gallery()
{
    delete ui;
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
    /*QImage image;
    image.load(fileName);
    ui->photoShow->setPixmap(QPixmap::fromImage(image));
    //label->setScaledContents(true);
    //label->resize(this->size());
    //ui->photoShow->resize(QSize(image.width(),image.height()));
    ui->photoShow->setGeometry((720-image.width())/2,(1280-image.height())/2,image.width(),image.height());
    ui->photoShow->show();*/

	qDebug() << " select file " << endl;
	vidPlayer->setVideoName(fileName.toStdString().c_str());
	vidPlayer->screenUpdateCb = updateScreen;
	vidPlayer->screenCleanCb = cleanScreen;
	vidPlayer->start();
	isFirstPlay = true;
	isPlaying = true;

}


void Gallery::on_BackButton_clicked()
{
	vidPlayer->stop();
	this->close();
    return;
}

void Gallery::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (isPlaying && vidFrame != NULL)
	{
		painter.fillRect(0, 0, width(), height(), Qt::black);
		QImage *tmp_img=new QImage((uchar *)vidFrame,vidWidth,vidHeight,QImage::Format_RGB888);
    	painter.drawImage(vidRect,*tmp_img);
	} else {
		this->close();
	}
}

void Gallery::updateScreen(uint8_t *frameRGB, int width, int height)
{
	pThis->vidWidth = width;
	pThis->vidHeight = height;
	pThis->vidFrame = frameRGB;
	pThis->repaint(pThis->vidRect);
}

void Gallery::cleanScreen()
{
	pThis->isPlaying = false;
	pThis->repaint();
}
