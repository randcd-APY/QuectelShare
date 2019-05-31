#include "qcamdemo.h"
#include "ui_qcamdemo.h"
#include "qicon.h"
#include "qpixmap.h"
#include "QPainter"
#include <QString>
#include <QFile>
#include <QDir>
#include <QDate>
#include "QIODevice"
#include "QByteArray"

#define USER_PIC_SIZE_WIDTH 3264
#define USER_PIC_SIZE_HEIGHT 2448
#define QT_VIDEO_WIDTH 640
#define QT_VIDEO_HEIGHT 480
#define JPEG_NAME_MAXLEN 256

#define QUEC_CAMERA_VIDEO_DIR "/data/misc/camera/video"
#define QUEC_CAMERA_PIC_DIR "/data/DCIM"

#ifdef PREVIEW_DUMP
	static int saveP = 0;
#endif

QCamDemo *pThisWindow = NULL;
QCamDemo::QCamDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QCamDemo)
{
    ui->setupUi(this);
    pThisWindow = this;
    mVideoRecoder = false; //is video recoder running
    thisCb = new CameraCallBack();
    thisCb->paintFinish = true;
    QCamUIInit();

    //search DCIM directory
    QDir targetDir(QUEC_CAMERA_PIC_DIR);
    if(!targetDir.exists()){    /**<if not exist, then create */
        targetDir.mkpath(targetDir.absolutePath());
    }
	if(thisCb->startCameraThread()){
		qDebug() << "QCAM_ERROR: camera thread start failed"<< endl;
		this->close();
	}
}

QCamDemo::~QCamDemo()
{
    delete ui;
}


void QCamDemo::QCamUIInit()
{
//    ui->CamExit->resize(100, 100);
//    ui->CamFunction->resize(100, 100);
//    ui->CamStart->resize(100, 100);
    ui->CamExit->setGeometry(50, 1180, 100, 100);
    ui->CamFunction->setGeometry(570, 1180, 100, 100);
    ui->CamStart->setGeometry(310, 1180, 100, 100);

    backbtn.addFile(tr(":/back.jpg"));
    startbtn.addFile(tr(":/take.jpg"));
    funcbtn.addFile(tr(":/video.jpg"));
    ui->CamExit->setIcon(backbtn);
    ui->CamFunction->setIcon(funcbtn);
    ui->CamStart->setIcon(startbtn);

}

void CameraCallBack::onPreviewFrame(ICameraFrame* frame)
{
	int ret;

#ifdef PREVIEW_DUMP
    char name[32];
    qDebug() << "QCAM_INFO: onPreviewFrame size:" << frame->size << "saveP :" << saveP << endl;
    //frame->size = 640*480*3/2;
    if(10 < saveP  && saveP < 15){
        qDebug() << "QCAM_INFO: dump frame size:" << frame->size << endl;
        snprintf(name, 32, "/data/misc/camera/P%d.yuv",saveP);
        dumpToFile(frame->data, frame->size, name, frame->timeStamp);
    }
    saveP ++;
#endif
	qDebug() << "QCAM_INFO: onPreviewFrame size:" << frame->size << endl;
    ret = pthread_mutex_trylock(&mutexPicDone);
    if(EBUSY == ret){
        qDebug() << "taking picture, return" << endl;
        return;
    }else{
        if(!isPicDone){
            qDebug() << "is taking picture, return" << endl;
            pthread_mutex_unlock(&mutexPicDone);
            return;
        }
    }

	char yuvRotateBuf[QT_PREVIEW_SIZE];
	memset(yuvRotateBuf, 0, sizeof(yuvRotateBuf));
	rotateYUV240SP((char *)frame->data, yuvRotateBuf, QT_PREVIEW_WIDTH, QT_PREVIEW_HEIGHT);
	NV21_TO_RGB24((unsigned char*)yuvRotateBuf, (unsigned char*)rgbBuf, QT_PREVIEW_HEIGHT, QT_PREVIEW_WIDTH);
	pThisWindow->update();
	pthread_mutex_unlock(&mutexPicDone);
}

void CameraCallBack::onVideoFrame(ICameraFrame* frame){
	qDebug() << "QCAM_INFO: onVideoFrame size:" << frame->size << endl;
	if (pThisWindow->mVideoRecoder)
    {
        pThisWindow->quecRecorderThread->quecBufInsert(frame);
    }
}

void CameraCallBack::onPictureFrame(ICameraFrame* frame)
{
    char imageName[JPEG_NAME_MAXLEN];
    QString JpegName = NULL;
	//qDebug() << "QCAM_INFO: onPictureFrame size:" << frame->size << endl;
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyyMMdd");
    QString current_time = current_date_time.toString("hhmmss");
	JpegName = JpegName.append(QUEC_CAMERA_PIC_DIR"/Q_").append(current_date).append("_").append(current_time).append(".jpg");
    qDebug() << JpegName << endl;
	std::string timestr = JpegName.toStdString();
	snprintf(imageName, 256, "%s",timestr.c_str());
    dumpToFile(frame->data, frame->size, imageName, frame->timeStamp);
    /* notify the waiting thread about picture done */
    pthread_mutex_lock(&mutexPicDone);
    isPicDone = true;
    pthread_cond_signal(&cvPicDone);
    pthread_mutex_unlock(&mutexPicDone);
}

void QCamDemo::paintEvent(QPaintEvent *)
{
    //qDebug() << "QINFO:paintEvent" << endl;
    QPainter painter(this);
    QRect rect = QRect(0, 75, 720, 960);
    QImage *tmp_img=new QImage((uchar *)thisCb->rgbBuf,QT_PREVIEW_HEIGHT,QT_PREVIEW_WIDTH,QImage::Format_RGB888);
    painter.drawImage(rect,*tmp_img);

}

void QCamDemo::on_CamExit_clicked()
{
	if(mVideoRecoder)
	{
		quecRecorderThread->quecRecorderRelease();
		thisCb->camera_->stopRecording();
	}
	thisCb->camera_->stopPreview();
	thisCb->camera_->deleteInstance(&(thisCb->camera_));
	this->close();
}

void QCamDemo::on_CamStart_clicked()
{
    if(isVideoGoing)
        return;
    qDebug() << "QINFO:on_CamStart_clicked: " << endl;
	thisCb->picSize_.width = USER_PIC_SIZE_WIDTH;
	thisCb->picSize_.height = USER_PIC_SIZE_HEIGHT;
    thisCb->params_.setPictureSize(thisCb->picSize_);
    //thisCb->params_.setPictureThumbNailSize(thisCb->picSize_);
    thisCb->params_.setExposureTime("0");
    thisCb->params_.setISO("auto");
    thisCb->params_.commit();
    if(thisCb->takePicture(1)){
        qDebug() << "QERROR:takePicture failed: " << endl;
    }

}

void QCamDemo::on_CamFunction_clicked()
{
    if(!mVideoRecoder){
    	//funcbtn.addFile(tr(":/recorder.jpg"));
        ui->CamFunction->setIcon(QIcon(":/recorder.jpg"));
        isVideoGoing = true;
        thisCb->camera_->stopPreview();
        thisCb->vSize_.width = QT_VIDEO_WIDTH;
        thisCb->vSize_.height = QT_VIDEO_HEIGHT;
        thisCb->params_.setVideoSize(thisCb->vSize_);
        thisCb->params_.commit();
        thisCb->camera_->startPreview();
        thisCb->camera_->startRecording();

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyyMMdd");
        QString current_time = current_date_time.toString("hhmmsszzz");
        if (access(QUEC_CAMERA_VIDEO_DIR, F_OK) != 0)
        {
            qDebug("create %s", QUEC_CAMERA_VIDEO_DIR);
            if (mkdir(QUEC_CAMERA_VIDEO_DIR, S_IRWXU) != 0)
            {
                 qFatal("failed to create %s", QUEC_CAMERA_VIDEO_DIR);
            }
        }
        QString video_name = tr(QUEC_CAMERA_VIDEO_DIR"/VID").append(current_date).append(current_time).append(".mp4");

        quecRecorderThread = new QuecRecorderThread();
        quecRecorderThread->quecRecorderInit((unsigned char *)video_name.toStdString().c_str(), thisCb->vSize_.width, thisCb->vSize_.height);
        quecRecorderThread->start();


		mVideoRecoder = true;
    } else{
		//funcbtn.addFile(tr(":/video.jpg"));
        ui->CamFunction->setIcon(QIcon(":/video.jpg"));
        mVideoRecoder = false;
        isVideoGoing = false;

        quecRecorderThread->quecRecorderRelease();
        thisCb->camera_->stopRecording();
    }
}

void QCamDemo::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "QINFO:mousePressEvent" << endl;

}

void QCamDemo::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "QINFO:mouseReleaseEvent" <<endl;
}
