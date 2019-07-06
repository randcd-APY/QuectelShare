#include "qcamdata.h"
#include <QtDebug>

const int TAKEPICTURE_TIMEOUT_MS = 5000;
#define MS_PER_SEC 1000
#define NS_PER_MS 1000000
#define NS_PER_US 1000

int CameraCallBack::startCameraThread(int camId)
{
    int g_num_of_cameras, rc;

    pthread_cond_init(&cvPicDone, NULL);
    pthread_mutex_init(&mutexPicDone, NULL);
	isPicDone = true;
    g_num_of_cameras = getNumberOfCameras();
    if(1 > g_num_of_cameras){
        qDebug() << "QCAM_ERROR: qcamera get num of cameras: " << g_num_of_cameras << endl;
		goto err;
    }else{
        qDebug() << "QCAM_INFO:qcamera get num of cameras: " << g_num_of_cameras << endl;
    }

    rc = ICameraDevice::createInstance(camId, &camera_);//open camera
    if (rc != 0) {
        qDebug() << "QCAM_ERROR: could not open camera: " << camId << "rc: " << rc << endl;
		goto err;
    }else{
        qDebug() << "QCAM_INFO: open camera: " << camId << "rc: " << rc << endl;
    }
	camera_->addListener(this);
    rc = params_.init(camera_);
    if (rc != 0) {
        printf("failed to init parameters\n");
        ICameraDevice::deleteInstance(&camera_);
        goto del_camera;
    }

	rc = setParameters(QT_PREVIEW_WIDTH, QT_PREVIEW_HEIGHT);
    if (rc) {
        qDebug() << "QCAM_ERROR: setParameters failed" <<endl;
        goto del_camera;
    }
    camera_->startPreview();
	return 0;

err:
	return -1;
del_camera:
    ICameraDevice::deleteInstance(&camera_);
    return rc;

}

int CameraCallBack::setParameters(int qwidth, int qheight)
{
    pSize_.width = qwidth;
    pSize_.height = qheight;
    params_.setWhiteBalance("auto");
    params_.setPreviewSize(pSize_);
	params_.setWhiteBalance("auto");
	params_.setISO("auto");
	//params_.setAntibanding("off");
    //params_.setStatsLoggingMask(0);
    qDebug() << "QCAM_INFO: setParameters:pwidth:" << pSize_.width << " pheight:" << pSize_.height << endl;
    return params_.commit();
}

int CameraCallBack::takePicture(uint32_t num_images)
{
    int rc;
    pthread_mutex_lock(&mutexPicDone);
    usleep(1000*100);//wait for preview frame blocking
    isPicDone = false;
    rc = camera_->takePicture(num_images);
    if (rc) {
        qDebug() << "QCAM_ERROR:takePicture failed" << endl;
        pthread_mutex_unlock(&mutexPicDone);
        return rc;
    }
    struct timespec waitTime;
    struct timeval now;

    gettimeofday(&now, NULL);
    waitTime.tv_sec = now.tv_sec + TAKEPICTURE_TIMEOUT_MS/MS_PER_SEC;
    waitTime.tv_nsec = now.tv_usec * NS_PER_US + (TAKEPICTURE_TIMEOUT_MS % MS_PER_SEC) * NS_PER_MS;
    /* wait for picture done */
    while (isPicDone == false) {
        rc = pthread_cond_timedwait(&cvPicDone, &mutexPicDone, &waitTime);
        if (rc == ETIMEDOUT) {
            qDebug() << "QCAM_ERROR: takePicture timed out" << endl;
            break;
        }
    }
    pthread_mutex_unlock(&mutexPicDone);
    return 0;	
}

int CameraCallBack::dumpToFile(uint8_t* data, uint32_t size, char* name, uint64_t timestamp)
{
    FILE* fp;
    fp = fopen(name, "wb");
    if (!fp) {
        printf("fopen failed for %s\n", name);
        return -1;
    }
    fwrite(data, size, 1, fp);
    printf("saved filename %s\n", name);
    fclose(fp);
    return 0;
}

void CameraCallBack::rotateYUV240SP(char *src,char *des,int width,int height, int camId)
{
	if(camId){
	   //NV21旋转90
	   int wh, i, j, k;
	   wh = width * height;
	   //旋转Y
	   k = 0;
	   for(i=0;i<width;i++) {
	       for(j=0;j<height;j++){
	           des[k] = src[width*j + i];
	           k++;
	       }
	    }
	
	    for(i=0;i<width;i+=2) {
	       for(j=0;j<height/2;j++){
	           des[k] = src[wh+ width*j + i];
	           des[k+1]=src[wh + width*j + i+1];
	           k+=2;
	       }
	    }
	}else{
	    //NV21旋转270
	   int y_size = width * height;
	   int i = 0;
	   // Rotate the Y luma
	   for (int x = width - 1; x >= 0; x--){
	   	int offset = 0;
	   	for (int y = 0; y < height; y++){
	   		des[i] = src[offset + x];
	   		i++;
	   		offset += width;
	   	}
	   }
	   // Rotate the U and V color components
	   i = y_size;
	   for (int x = width - 1; x > 0; x = x - 2)
	   {
	   	int offset = y_size;
	   	for (int y = 0; y < height / 2; y++){
	   		des[i] = src[offset + (x - 1)];
	   		i++;
	   		des[i] = src[offset + x];
	   		i++;
	   		offset += width;
	   	}
	   }
	}
}

void CameraCallBack::NV21_TO_RGB24(unsigned char *yuyv, unsigned char *rgb, int width, int height) 
{
	const int nv_start = width * height ;
    int  i, j, index = 0, rgb_index = 0;
    int y, u, v;
    int r, g, b, nv_index = 0;
	
 
    for(i = 0; i <  height ; i++)
    {
		for(j = 0; j < width; j ++){
			//nv_index = (rgb_index / 2 - width / 2 * ((i + 1) / 2)) * 2;
			nv_index = i / 2  * width + j - j % 2;
 
			y = yuyv[rgb_index];
			u = yuyv[nv_start + nv_index ];
			v = yuyv[nv_start + nv_index + 1];
			
		
			b = y + 1.4*(v-128);//(140 * (v-128))/100;  //r
			g = y - (0.34*(u-128)) - (0.71*(v-128));//y - (34 * (u-128))/100 - (71 * (v-128))/100; //g
			r = y + 1.77*(u-128);//y + (177 * (u-128))/100; //b
				
			if(r > 255)   r = 255;
			if(g > 255)   g = 255;
			if(b > 255)   b = 255;
       		if(r < 0)     r = 0;
			if(g < 0)     g = 0;
			if(b < 0)     b = 0;
			
			index = rgb_index % width + (height - i - 1) * width;
			rgb[index * 3+0] = r;
			rgb[index * 3+1] = g;
			rgb[index * 3+2] = b;
			rgb_index++;
		}
    }
}

