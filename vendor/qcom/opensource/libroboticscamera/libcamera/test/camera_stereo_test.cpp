/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <assert.h>
#include "camera.h"
#include "camera_log.h"
#include "camera_parameters.h"
#include "camera_util_stereo.h"



#define DEFAULT_EXPOSURE_VALUE  250
#define MIN_EXPOSURE_VALUE 0
#define MAX_EXPOSURE_VALUE 65535
#define DEFAULT_GAIN_VALUE  50
#define MIN_GAIN_VALUE 0
#define MAX_GAIN_VALUE 255
#define QCAMERA_DUMP_LOCATION "/CAMdata/"


#define DEFAULT_CAMERA_FPS 30
#define MS_PER_SEC 1000
#define NS_PER_MS 1000000
#define NS_PER_US 1000



using namespace std;
using namespace camera;

struct CameraCaps
{
    vector<ImageSize> pSizes;
    vector<Range> previewFpsRanges;
    vector<VideoFPS> videoFpsValues;
    vector<string> previewFormats;
    string rawSize;
};

enum OutputFormatType{
    YUV_FORMAT,
    RAW_FORMAT,
};

enum SensorIndex{
    LEFT_SENSOR_INDEX = 0,
    RIGHT_SENSOR_INDEX = 1,
    MAX_SENSOR_INDEX = 2,
};

enum AppLoglevel {
    CAM_LOG_SILENT = 0,
    CAM_LOG_ERROR = 1,
    CAM_LOG_INFO = 2,
    CAM_LOG_DEBUG = 3,
    CAM_LOG_MAX,
};

#define DEFAULT_EXPOSURE_VALUE  250
#define MIN_EXPOSURE_VALUE 0
#define MAX_EXPOSURE_VALUE 65535
#define DEFAULT_GAIN_VALUE  50
#define MIN_GAIN_VALUE 0
#define MAX_GAIN_VALUE 255
#define QCAMERA_DUMP_LOCATION "/CAMdata/"


#define DEFAULT_CAMERA_FPS 30
#define MS_PER_SEC 1000
#define NS_PER_MS 1000000
#define NS_PER_US 1000

/**
*  Helper class to store all parameter settings
*/
struct TestConfig {
    bool dumpFrames;
    bool infoMode;
    int runTime;
    int exposureValue;
    int gainValue;
	OutputFormatType outputFormat;
    ImageSize pSize;
    int fps;
    AppLoglevel logLevel;
    int storagePath;
    int statsLogMask;
};

/**
 * CLASS  CameraTest
 *
 * - inherits ICameraDeviceArrayListener which 
 *   provides core functionality
 * - User must define onPreviewFrame (virtual) function. It is
 *    the callback function for every preview frame.
 * - If any error occurs,  onError() callback function is
 *    called. User must define onError if error handling is
 *    required.
 */
class CameraTest : public ICameraDeviceArrayListener {
public:

    CameraTest();
    CameraTest(TestConfig config);
    ~CameraTest();
    int run();

    /* listener methods */
    virtual void onError();
    virtual void onPreviewFrame(std::vector<ICameraFrame *> frames);
    virtual void onVideoFrame(std::vector<ICameraFrame *> frames);
    virtual void onPictureFrame(std::vector<ICameraFrame *> frames);

private:
    ICameraDeviceArray *stereoDeviceArray_;
    CameraParams params_;
    ImageSize pSize_;
    CameraCaps caps_;
    TestConfig config_;
	
    uint32_t pFrameCount_;
    float vFpsAvg_, pFpsAvg_;
    uint64_t vTimeTotal_, pTimeTotal_;
    uint64_t vTimeStampPrev_, pTimeStampPrev_;

    int initialize();
    int setParameters();
    int setFPSindex(int fps, int &pFpsIdx, int &vFpsIdx);
};


CameraTest::CameraTest() :
    pFpsAvg_(0.0f),
    pFrameCount_(0),
    pTimeStampPrev_(0) {
}

CameraTest::CameraTest(TestConfig config) :
    pFpsAvg_(0.0f),
    pFrameCount_(0),
    pTimeStampPrev_(0) {
    config_ = config;
}

CameraTest::~CameraTest() {
}
int CameraTest::initialize()
{

    cout << "Creating the stereo device " << endl;


    /* Initializes left and right camera sensors and params for both the sensors */
    stereoDeviceArray_ = StereoCameraUtilFactory::createStereoDevice(&params_);
    if (nullptr == stereoDeviceArray_) {
        cerr << "Error: Could not create stereo sensor";
        return 1;
    }
	
    caps_.pSizes = params_.getSupportedPreviewSizes();
    caps_.previewFpsRanges = params_.getSupportedPreviewFpsRanges();
    caps_.videoFpsValues = params_.getSupportedVideoFps();
    caps_.previewFormats = params_.getSupportedPreviewFormats();
    caps_.rawSize = params_.get("raw-size");

    return EXIT_SUCCESS;
}

void CameraTest::onError() {
    cerr << "camera error!, aborting";
    exit(EXIT_FAILURE);
}


static int dumpToFile(uint8_t *data, uint32_t size, char *name, uint64_t timestamp) {
    FILE *fp;
    fp = fopen(name, "wb");
    if (!fp) {
        printf("fopen failed for %s\n", name);
        return -1;
    }
    fwrite(data, size, 1, fp);
    printf("saved filename %s\n",name);
    fclose(fp);
    return EXIT_SUCCESS;
}
/**
 *
 * FUNCTION: onPreviewFrame
 *
 *  - This is called every frame I
 *  - In the test app, we save files only after every 30 frames
 *  - In parameter frame (ICameraFrame) also has the timestamps
 *    field which is public
 *
 * @param frame
 *
 */
void CameraTest::onPreviewFrame(std::vector<ICameraFrame *> frames) {
    
    char name[50];
    int time_diff = 0;
    char format_str[10];

    if ((pFrameCount_ % 30 == 0) && (config_.dumpFrames == true)) {

        if (config_.outputFormat == YUV_FORMAT) {
            strcpy(format_str,"yuv");
        }else{
            strcpy(format_str,"raw");
        }

        if (config_.storagePath == 1) {
            snprintf(name, 50, QCAMERA_DUMP_LOCATION "stereo_%04d_l_%dx%d_%llu.%s",pFrameCount_,
                     config_.pSize.width, config_.pSize.height, frames[0]->timeStamp,format_str);
        } else {
            snprintf(name, 50, "stereo_%04d_l_%dx%d_%llu.%s", pFrameCount_,
                     config_.pSize.width, config_.pSize.height, frames[0]->timeStamp,format_str);
        }
        dumpToFile(frames[0]->data, frames[0]->size, name, frames[0]->timeStamp);

        if (config_.storagePath == 1) {
            snprintf(name, 50, QCAMERA_DUMP_LOCATION "stereo_%04d_r_%dx%d_%llu.%s", pFrameCount_,
                     config_.pSize.width, config_.pSize.height, frames[1]->timeStamp, format_str);
        } else {
            snprintf(name, 50, "stereo_%04d_r_%dx%d_%llu.%s", pFrameCount_, 
                     config_.pSize.width, config_.pSize.height, frames[1]->timeStamp,format_str);
        }
        dumpToFile(frames[1]->data, frames[1]->size, name, frames[1]->timeStamp);

        #ifndef LEFT_RIGHT_DELTA
            time_diff =  int ( llabs(frames[0]->timeStamp - frames[1]->timeStamp) / 1000); 
            printf(" left frame : ts = %llu  right frame : ts = %llu  diff = %d uS\n", frames[0]->timeStamp , frames[1]->timeStamp ,time_diff );
        #endif

    }
    #ifdef LEFT_RIGHT_DELTA
            time_diff =  int ( llabs(frames[0]->timeStamp - frames[1]->timeStamp) / 1000);
            printf(" left frame : ts = %llu  right frame : ts = %llu  diff = %d uS\n", frames[0]->timeStamp , frames[1]->timeStamp ,time_diff );
    #endif
    
    uint64_t diff = frames[0]->timeStamp - pTimeStampPrev_;
    if (pTimeStampPrev_ != 0)
      pTimeTotal_ += diff;
    pFpsAvg_ = ((pFpsAvg_ * pFrameCount_) + (1e9 / diff)) / (pFrameCount_ + 1);
    pFrameCount_++;
    pTimeStampPrev_  = frames[0]->timeStamp;

}

void CameraTest::onPictureFrame(std::vector<ICameraFrame *> frames) {
}


void CameraTest::onVideoFrame(std::vector<ICameraFrame *> frames) {
}



ImageSize VGASize(640,480);
ImageSize QVGASize(320,240);
const char usageStr[] =
    "Camera API test application \n"
    "\n"
    "usage: camera-test [options]\n"
    "\n"
    "  -t <duration>   capture duration in seconds [10]\n"
    "  -d              dump frames\n"
    "  -i              info mode\n"
    "                    - print camera capabilities\n"
    "                    - streaming will not be started\n"
    "  -p <size>       Set resolution for preview frame\n"
    "                    - VGA            ( Max resolution of optic flow and right sensor )\n"
    "                    - QVGA           ( 320x240 ) \n"
    "  -e <value>      set exposure control (only for ov7251)\n"
    "                     min - 0\n"
    "                     max - 65535\n"
    "  -g <value>      set gain value (only for ov7251)\n"
    "                     min - 0\n"
    "                     max - 255\n"
    "  -r < value>     set fps value      (Enter supported fps for requested resolution) \n"
    "                    -  30 (default)\n"
    "                    -  60 \n"
    "                    -  90 \n"
    "  -o <value>      Output format\n"
    "                     0 :YUV format \n"
    "                     1 : RAW format (default)\n"
    "  -V <level>      syslog level [0]\n"
    "                    0: silent\n"
    "                    1: error\n"
    "                    2: info\n"
    "                    3: debug\n"
    " -S <MASK>         Enable stats log\n"
    "                    0x00:  STATS_NO_LOG , ( Default )\n"
    "                    0x01:  STATS_AEC_LOG_MASK  (1 << 0)\n"
    "                    0x02:  STATS_AWB_LOG_MASK  (1 << 1)\n"
    "                    0x04:  STATS_AF_LOG_MASK   (1 << 2)\n"
    "                    0x08:  STATS_ASD_LOG_MASK  (1 << 3)\n"
    "                    0x10:  STATS_AFD_LOG_MASK  (1 << 4)\n"
    "                    0x1F:  STATS_ALL_LOG\n"
    "  -h              print this message\n"
;

static inline void printUsageExit(int code) {
    printf("%s", usageStr);
    exit(code);
}
/**
 * FUNCTION: setFPSindex
 *
 * scans through the supported fps values and returns index of
 * requested fps in the array of supported fps
 *
 * @param fps      : Required FPS  (Input)
 * @param pFpsIdx  : preview fps index (output)
 * @param vFpsIdx  : video fps index   (output)
 *
 *  */
int CameraTest::setFPSindex(int fps, int &pFpsIdx, int &vFpsIdx)
{
    int defaultPrevFPSIndex = -1;
    int defaultVideoFPSIndex = -1;
    int i,rc = 0;
    for (i = 0; i < caps_.previewFpsRanges.size(); i++) {
        if (  (caps_.previewFpsRanges[i].max)/1000 == fps )
        {
            pFpsIdx = i;
            break;
        }
        if ( (caps_.previewFpsRanges[i].max)/1000 == DEFAULT_CAMERA_FPS )
        {
            defaultPrevFPSIndex = i;
        }
    }
    if ( i >= caps_.previewFpsRanges.size() )
    {
        if (defaultPrevFPSIndex != -1 )
        {
            pFpsIdx = defaultPrevFPSIndex;
        } else
        {
            pFpsIdx = -1;
            rc = -1;
        }
    }

    for (i = 0; i < caps_.videoFpsValues.size(); i++) {
        if ( fps == 30 * caps_.videoFpsValues[i])
        {
            vFpsIdx = i;
            break;
        }
        if ( DEFAULT_CAMERA_FPS == 30 * caps_.videoFpsValues[i])
        {
            defaultVideoFPSIndex = i;
        }
    }
    if ( i >= caps_.videoFpsValues.size())
    {
        if (defaultVideoFPSIndex != -1)
        {
            vFpsIdx = defaultVideoFPSIndex;
        }else
        {
            vFpsIdx = -1;
            rc = -1;
        }
    }
    return rc;
}
int CameraTest::setParameters() {

    int pFpsIdx = -1;
    int vFpsIdx = -1;
    int rc = 0;

    pSize_ = config_.pSize;
    if (config_.outputFormat == RAW_FORMAT) {
        printf("Setting output Format = RAW_FORMAT \n");
        params_.set("preview-format", "bayer-rggb");
        params_.set("picture-format", "bayer-mipi-10gbrg");
        params_.set("raw-size", "640x480");
    }else{
        printf("Setting output Format = YUV_FORMAT \n");
    }

    printf("setting preview size: %dx%d\n", pSize_.width, pSize_.height);
    params_.setPreviewSize(pSize_);
    printf("setting video size: %dx%d\n", pSize_.width, pSize_.height);
    params_.setVideoSize(pSize_);

    /* Find index and set FPS  */
    rc = setFPSindex(config_.fps, pFpsIdx, vFpsIdx);
    if ( rc == -1)
    {
        return rc;
    }
    printf("setting preview fps range: %d, %d ( idx = %d ) \n",
    caps_.previewFpsRanges[pFpsIdx].min,
    caps_.previewFpsRanges[pFpsIdx].max, pFpsIdx);
    params_.setPreviewFpsRange(caps_.previewFpsRanges[pFpsIdx]);
    printf("setting video fps: %d ( idx = %d )\n", caps_.videoFpsValues[vFpsIdx], vFpsIdx );
    params_.setVideoFPS(caps_.videoFpsValues[vFpsIdx]);
  
	
    params_.setStatsLoggingMask(config_.statsLogMask);

    /* Instead of params.commit ICameraDeviceArray will perform the commit on both the sensors */
    return stereoDeviceArray_->setParameters(params_);
}

int CameraTest::run()
{  
    int rc = EXIT_SUCCESS;

    /* finding correct cmdId is done within  StereoCameraUtilFactory::createStereoDevice */
	initialize();
    setParameters();
    if (rc) {
        printf("setParameters failed\n");
        printUsageExit(0);
        goto del_camera;
    } 

    cout << "Adding listener" << endl;
    stereoDeviceArray_->addListener(this);
    cout << "Starting preview" << endl;
    stereoDeviceArray_->startPreview();

   /* Set Parameters after starting preview */

    params_.setManualExposure(config_.exposureValue);
    params_.setManualGain(config_.gainValue);
    params_.setVerticalFlip(true);
    params_.setHorizontalMirror(true);
    stereoDeviceArray_->setParameters(params_);

    printf("Setting exposure value =  %d , gain value = %d \n", config_.exposureValue, config_.gainValue );

    cout << "Sleeping for " << config_.runTime << endl;
    sleep(config_.runTime);


    cout << "Stopping preview " << endl;
    stereoDeviceArray_->stopPreview();
    cout << "Removing listener" << endl;
    stereoDeviceArray_->removeListener(this);
    cout << "Done!" << endl;

    printf("Average preview FPS = %.2f\n", pFpsAvg_);

del_camera:
    delete stereoDeviceArray_;
    return rc;
}

/**
 *  FUNCTION: setDefaultConfig
 *
 *  set default config based on camera module
 *
 * */
static int setDefaultConfig(TestConfig &cfg) {

    cfg.outputFormat = RAW_FORMAT;
    cfg.dumpFrames = false;
    cfg.runTime = 10;
    cfg.infoMode = false;
    cfg.exposureValue = DEFAULT_EXPOSURE_VALUE;  /* Default exposure value */
    cfg.gainValue = DEFAULT_GAIN_VALUE;  /* Default gain value */
    cfg.fps = DEFAULT_CAMERA_FPS;
    cfg.logLevel = CAM_LOG_SILENT;
    cfg.statsLogMask = STATS_NO_LOG;
    cfg.storagePath = 0;
    cfg.pSize = VGASize;

    return EXIT_SUCCESS;
}
/**
 *  FUNCTION: parseCommandline
 *
 *  parses commandline options and populates the config
 *  data structure
 *
 *  */
static TestConfig parseCommandline(int argc, char *argv[]) {
    TestConfig cfg;
    int c;
    int outputFormat;
    int exposureValueInt = 0;
    int gainValueInt = 0;

    setDefaultConfig(cfg);

    while ((c = getopt(argc, argv, "hdo:e:g:p:r:V:t:iS:P:")) != -1) {
        switch (c) {
        case 't':
            cfg.runTime = atoi(optarg);
            break;
        case 'p':
            {
                string str(optarg);
                if (str == "VGA") {
                    cfg.pSize = VGASize;
                } else if (str == "QVGA") {
                    cfg.pSize = QVGASize;
                } else{
                    cerr << "Error: invalid preview size\n";
                }
                break;
            }
        case 'd':
            cfg.dumpFrames = true;
            break;
        case 'i':
            cfg.infoMode = true;
            break;
        case  'e':
            cfg.exposureValue =  atoi(optarg);
            if (cfg.exposureValue < MIN_EXPOSURE_VALUE || cfg.exposureValue > MAX_EXPOSURE_VALUE) {
                cout << "Invalid exposure value. Using default\n";
                cfg.exposureValue = DEFAULT_EXPOSURE_VALUE;
            }
            break;
        case  'g':
            cfg.gainValue =  atoi(optarg);
            if (cfg.gainValue < MIN_GAIN_VALUE || cfg.gainValue > MAX_GAIN_VALUE) {
                cout << "Invalid exposure value. Using default\n";
                cfg.gainValue = DEFAULT_GAIN_VALUE;
            }
            break;
        case 'r':
            cfg.fps = atoi(optarg);
            if (!(cfg.fps == 30 || cfg.fps == 60 || cfg.fps == 90 || cfg.fps == 120)) {
                cfg.fps = DEFAULT_CAMERA_FPS;
                cout << "Invalid fps values. Using default = " <<  cfg.fps;
            }
            break;
        case 'o':
            outputFormat = atoi(optarg);
            switch (outputFormat) {
            case 0: 
                cfg.outputFormat = YUV_FORMAT;
                break;
            case 1: /* optic only */
                cfg.outputFormat = RAW_FORMAT;
                break;
            default:
                printf("Invalid format. Setting to default YUV_FORMAT");
                cfg.outputFormat = YUV_FORMAT;
                break;
            }
            break;
        case 'V':
            cfg.logLevel = (AppLoglevel)atoi(optarg);
            break;
        case 'S':
            cfg.statsLogMask = (int)strtol(optarg, NULL, 0);
            break;
        case 'P':
            cfg.storagePath = (int)atoi(optarg);
            switch (cfg.storagePath) {
            case 0: /* Default Path */
                cout << "Default Path\n";
                break;
            case 1:
                if (access(QCAMERA_DUMP_LOCATION, 0) == 0) cout << QCAMERA_DUMP_LOCATION << " already created!\n";
                else {
                    int status = mkdir(QCAMERA_DUMP_LOCATION, 0666);
                    if (status == 0) cout << "created " << QCAMERA_DUMP_LOCATION;
                    else {
                        cout << "create storage path failed, Setting to default path\n";
                        cfg.storagePath = 0;
                    }
                }
                break;
            default:
                cout << "Invalid storage path. Setting to default path\n";
                cfg.storagePath = 0;
                break;
            }
            break;
        case 'h':
        case '?':
            printUsageExit(0);

        default:
            abort();
        }
    }
    return cfg;
}

int main(int argc, char *argv[]) {

    TestConfig config = parseCommandline(argc, argv);
    CameraTest stereo_test(config);

    stereo_test.run();


    return EXIT_SUCCESS;
}
