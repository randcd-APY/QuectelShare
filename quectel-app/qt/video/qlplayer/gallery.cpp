#include <QPainter>
#include <QHBoxLayout>
#include "gallery.h"
#include "ui_gallery.h"
 #include <unistd.h>

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
#include <stdio.h>

GETPPATH *newGetPath = NULL;
Gallery *pThis;

Gallery::Gallery(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gallery),
	isPlaying(false),
	videoPlayer(new VideoPlayer())
{
    ui->setupUi(this);
    pThis = this;

    vidRect = QRect(0, 240, 720, 480);
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

	qDebug() << " select file " << endl;
	videoPlayer->initVideoPlayer();
	videoPlayer->setVideoName(fileName.toStdString().c_str());
	videoPlayer->setVidUpdateCb(updateScreen);
	videoPlayer->setVidCleanCb(cleanScreen);
	videoPlayer->start();
	isFirstPlay = true;
	isPlaying = true;
}


void Gallery::Close()
{
	videoPlayer->stop();
	videoPlayer->quit();
	videoPlayer->wait();
	this->close();
	return ;
}

void Gallery::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (isPlaying && vidFrame != NULL)
	{
		QImage tmp_img = QImage((uchar *)vidFrame,vidWidth,vidHeight,QImage::Format_ARGB32);
   		//painter.drawImage(vidRect,tmp_img.rgbSwapped().mirrored(false, true));  //旋转
   		painter.drawImage(vidRect,tmp_img.mirrored(false, true));  //旋转180
	}
}

void Gallery::updateScreen(uint8_t *frame, int width, int height)
{
	pThis->vidWidth = width;
    pThis->vidHeight = height;
    pThis->vidFrame = frame;
    pThis->repaint(pThis->vidRect);
}

void Gallery::cleanScreen()
{
	qDebug("clean Screen");
	pThis->isPlaying = false;
	pThis->close();
}
