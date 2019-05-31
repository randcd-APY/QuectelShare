#include <QPainter>
#include <QHBoxLayout>
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
#include <stdio.h>

GETPPATH *newGetPath = NULL;
Gallery *pThis;
//FILE *fp;

Gallery::Gallery(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gallery),
	isPlaying(false),
	videoPlayer(new VideoPlayer())
{
    ui->setupUi(this);
	pThis = this;

	glWidget = new GLYuvToRgb();
	//fp = fopen("/data/vga.yuv", "r+");

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
    QImage image;
    image.load(fileName);
    ui->photoShow->setPixmap(QPixmap::fromImage(image));
    //label->setScaledContents(true);
    //label->resize(this->size());
    //ui->photoShow->resize(QSize(image.width(),image.height()));
    ui->photoShow->setGeometry((720-image.width())/2,(1280-image.height())/2,image.width(),image.height());
    ui->photoShow->show();

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
	qDebug("videoPlayer stop");
	videoPlayer->stop();
	videoPlayer->wait();
	qDebug("videoPlayer success");
	//fclose(fp);
	delete glWidget;
	this->close();
	return ;
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

void Gallery::updateScreen(uint8_t *frame, int width, int height)
{
	/*pThis->vidWidth = width;
	pThis->vidHeight = height;
	pThis->vidFrame = frame;
	pThis->repaint(pThis->vidRect);*/
	//qDebug("updateScreen frame = %p width = %d height = %d\n", frame, width, height);
	/*int length;
	length = fread(frame, 1, width*height*3/2, fp);
	if (length != 0)
	{
		printf("length = %d\n", length);
		pThis->glWidget->slotYuv(frame, width, height);
	}*/
}

void Gallery::cleanScreen()
{
	pThis->isPlaying = false;
	qDebug("clean screen");
	pThis->repaint();
}
