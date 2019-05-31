/* Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.
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
 *
 */
#ifndef __CAMERA_PARAMETERS_H__
#define __CAMERA_PARAMETERS_H__

#include <vector>
#include <string>

#include "camera.h"

namespace camera
{
/**
 * Image frame dimensions
 */
struct ImageSize
{
    int width;  /*!< Image width in pixels */
    int height; /*!< Image height in pixels */

    ImageSize() : width(0), height(0) {}
    ImageSize(int w, int h) : width(w), height(h) {}
};

/**
 * Structure for storing values for ranged parameters such as
 * brightness, contrast, fps etc.
 */
struct Range
{
    int min;    /*!< minimum value */
    int max;    /*!< maximum value */
    int step;   /*!< step increment for intermediate values */

    Range() : min(0), max(0), step(0) {}
    Range(int m, int x, int s) : min(m), max(x), step(s) {}
};

struct Range64
{
    uint64_t min;    /*!< minimum value */
    uint64_t max;    /*!< maximum value */
    uint64_t step;   /*!< step increment for intermediate values */

    Range64() : min(0), max(0), step(0) {}
    Range64(uint64_t m, uint64_t x, uint64_t s) : min(m), max(x), step(s) {}
};

/**
 * Available values for video FPS
 * 1X, 2X, 3X describes the video FPS values relative to preview
 * FPS. With video fps set to 3X, the video will run 3X the
 * speed of preview. This is also called HFR (High FrameRate)
 * mode
 * */
enum VideoFPS
{
    VIDEO_FPS_1X = 1,   /*!< 1x regular framerate mode */
    VIDEO_FPS_2X = 2,   /*!< 2x High framerate mode */
    VIDEO_FPS_3X = 3,   /*!< 3x High framerate mode */
    VIDEO_FPS_4X = 4, /*!< 4x High framerate mode */
    VIDEO_FPS_5X = 5, /*!< 5x High framerate mode */
};

/* focus mode values */
const std::string FOCUS_MODE_AUTO = "auto";
const std::string FOCUS_MODE_INFINITY = "infinity";
const std::string FOCUS_MODE_MACRO = "macro";
const std::string FOCUS_MODE_FIXED = "fixed";
const std::string FOCUS_MODE_EDOF = "edof";
const std::string FOCUS_MODE_CONTINUOUS_VIDEO = "continuous-video";
const std::string FOCUS_MODE_CONTINUOUS_PICTURE = "continuous-picture";
const std::string FOCUS_MODE_MANUAL_POSITION = "manual";
const std::string FOCUS_MODE_OFF = "off";

const std::string FORMAT_NV12 = "nv12";
const std::string FORMAT_NV12_VENUS = "nv12-venus";
const std::string FORMAT_NV21 = "yuv420sp";
const std::string FORMAT_RAW10 = "raw10";
const std::string FORMAT_RAW12 = "raw12";
const std::string FORMAT_JPEG = "jpeg";


/* ISO values */
const std::string ISO_AUTO = "auto";
const std::string ISO_HJR = "ISO_HJR";
const std::string ISO_100 = "ISO100";
const std::string ISO_200 = "ISO200";
const std::string ISO_400 = "ISO400";
const std::string ISO_800 = "ISO800";
const std::string ISO_1600 = "ISO1600";
const std::string ISO_3200 = "ISO3200";

/* White Balance values */
const std::string WHITE_BALANCE_OFF = "off";
const std::string WHITE_BALANCE_AUTO = "auto";
const std::string WHITE_BALANCE_INCANDESCENT = "incandescent";
const std::string WHITE_BALANCE_FLUORESCENT = "fluorescent";
const std::string WHITE_BALANCE_WARM_FLUORESCENT = "warm-fluorescent";
const std::string WHITE_BALANCE_DAYLIGHT = "daylight";
const std::string WHITE_BALANCE_CLOUDY_DAYLIGHT = "cloudy-daylight";
const std::string WHITE_BALANCE_TWILIGHT = "twilight";
const std::string WHITE_BALANCE_SHADE = "shade";
const std::string WHITE_BALANCE_MANUAL_CCT = "manual-cct";

 /* Sharpness values*/   // Used in HAL3 based implementation
const std::string SHARPNESS_MODE_EDGE_OFF = "off";
const std::string SHARPNESS_MODE_EDGE_FAST = "fast";
const std::string SHARPNESS_MODE_EDGE_HIGH_QUALITY = "highquality";
const std::string SHARPNESS_MODE_EDGE_ZERO_SHUTTER_LAG = "zsl";

/* ToneMap values*/      // Used in HAL3 based implementation
const std::string TONEMAP_CONTRAST_CURVE = "contrast";
const std::string TONEMAP_FAST = "fast";
const std::string TONEMAP_HIGH_QUALITY = "highquality";
const std::string TONEMAP_GAMMA_VALUE = "gamma";
const std::string TONEMAP_PRESET_CURVE = "preset-curve";

#define CAM_MAX_TONEMAP_CURVE_SIZE    512

typedef struct {
    /* A 1D array of pairs of floats.
     * Mapping a 0-1 input range to a 0-1 output range.
     * The input range must be monotonically increasing with N,
     * and values between entries should be linearly interpolated.
     * For example, if the array is: [0.0, 0.0, 0.3, 0.5, 1.0, 1.0],
     * then the input->output mapping for a few sample points would be:
     * 0 -> 0, 0.15 -> 0.25, 0.3 -> 0.5, 0.5 -> 0.64 */
    float tonemap_points[CAM_MAX_TONEMAP_CURVE_SIZE][2];
} cam_tonemap_curve_t;

typedef struct {
   size_t tonemap_points_cnt;
   cam_tonemap_curve_t curves[3];
}Tonemap_RBG;


enum StatsLoggingMask {
    STATS_NO_LOG           = 0,
    STATS_AEC_LOG_MASK     = (1 << 0),
    STATS_AWB_LOG_MASK     = (1 << 1),
    STATS_AF_LOG_MASK      = (1 << 2),
    STATS_ASD_LOG_MASK     = (1 << 3),
    STATS_AFD_LOG_MASK     = (1 << 4),
    STATS_ALL_LOG          = 0x1F,
};


class CameraParams : public ICameraParameters
{
public:

    CameraParams();

    virtual ~CameraParams();

    /**
     * initialize the object by getting current state of parameters
     * from device.
     *
     * @param device : A valid camera device object
     *
     * @return int : 0 on success
     */
    virtual int init(ICameraDevice* device);

    virtual int writeObject(std::ostream& ps) const;
    
    /**
     * get a string representation of the object
     *
     * @return std::string
     */
    std::string toString() const;

    /**
     * Updates the current state of the parameters to camera device.
     * Fails for any invalid entries.
     *
     * @return int : 0 on success
     */
    virtual int commit();

    /**
     * get preview sizes supported by the camera
     *
     * @return std::vector<ImageSize> : list of preview sizes
     */
    std::vector<ImageSize> getSupportedPreviewSizes() const;

    /**
     * set preview size
     *
     * @param size
     */
    void setPreviewSize(const ImageSize& size);

    /**
     * get current preview size
     *
     * @return ImageSize
     */
    ImageSize getPreviewSize() const;

    /**
     * get video sizes supported by the camera
     *
     * @return std::vector<ImageSize> : list of video sizes
     */
    std::vector<ImageSize> getSupportedVideoSizes() const;

    /**
     * get current video size
     *
     * @return ImageSize
     */
    ImageSize getVideoSize() const;

    /**
     * set video size
     *
     * @param size
     */
    void setVideoSize(const ImageSize& size);

    /**
     * get picture sizes supported by the camera
     *
     * @return std::vector<ImageSize> : list of picture sizes
     */
    std::vector<ImageSize> getSupportedPictureSizes(std::string format = FORMAT_JPEG) const;

    /**
     * get current picture size
     *
     * @return ImageSize
     */
    ImageSize getPictureSize() const;

    /**
     * set picture size
     *
     * see @ref takePicture
     *
     * @param size
     */
    void setPictureSize(const ImageSize& size);

    /**
     * get picture thumbnail size
     *
     * @return ImageSize
     */
    ImageSize getPictureThumbNailSize() const;

    /**
     * set picture thumbnail size
     *
     * @param size
     */
    void setPictureThumbNailSize(const ImageSize& size);

    /**
     * generic get function to get string representation of value of
     * a parameter using a key.
     *
     * @param key [in]
     *
     * @return std::string : value
     */
    virtual std::string get(const std::string& key) const;


    /**
     * generic set function to set value of a parameter using
     * key-value pair
     *
     * @param key [in]
     * @param value [in]
     */
    virtual void set(const std::string& key, const std::string& value);


    /**
     * get a list of supported focus modes
     *
     * @return vector<string> : focus mode values
     */
    std::vector<std::string> getSupportedFocusModes() const;

    /**
     * get current value of focus mode
     *
     * @return std::string
     */
    std::string getFocusMode() const;

    /**
     * set focus mode value
     *
     * @param value
     */
    void setFocusMode(const std::string& value);

    /**
     * get a list of supported whitebalance modes
     *
     * @return vector<string> : whitebalance values
     */
    std::vector<std::string> getSupportedWhiteBalance() const;

    /**
     * get current value of whitebalance mode
     *
     * @return std::string
     */
    std::string getWhiteBalance() const;

    /**
     * set whitebalance mode value
     *
     * @param value
     */
    void setWhiteBalance(const std::string& value);

    /**
     * get a list of supported ISO modes
     *
     * @return vector<string> : ISO values
     */
    std::vector<std::string> getSupportedISO() const;

    /**
     * get current value of ISO mode
     *
     * @return std::string
     */
    std::string getISO() const;

    /**
     * set ISO mode value
     *
     * @param value
     */
    void setISO(const std::string& value);

    /**
     * get a list of supported Sharpness modes
     *
     * (HAL3 only)
     *
     * @return vector<string> : Sharpness values
     */
    std::vector<std::string> getSupportedSharpnessMode() const;

    /**
     * get current value of Sharpness mode
     *
     * (HAL3 only)
     *
     * @return std::string
     */
    std::string getSharpnessMode() const;

    /**
     * set Sharpness mode value
     *
     * (HAL3 only)
     *
     * @param value
     */
    void setSharpnessMode(const std::string& value);

    /**
     * get a list of supported ToneMap modes
     *
     * (HAL3 only)
     *
     * @return vector<string> : Sharpness values
     */
    std::vector<std::string> getSupportedToneMapMode() const;

    /**
     * get current value of ToneMap mode
     *
     * (HAL3 only)
     *
     * @return std::string
     */
    std::string getToneMapMode() const;

    /**
     * set ToneMap mode value
     *
     * (HAL3 only)
     *
     * @param value
     */
    void setToneMapMode(const std::string& value);    

    /**
     * get a range of supported sharpness values
     *
     *(HAL3 only)
     *
     * @return Range : sharpness range
     */
    Range getSupportedSharpness() const;

    /**
     * get current sharpness value
     *
     * @return int
     */
    int getSharpness() const;

    /**
     * set sharpness value
     *
     * @param value
     */
    void setSharpness(int value);

    /**
     * get a range of supported brightness values
     *
     * @return Range : brightness range
     */
    Range getSupportedBrightness() const;

    /**
     * get current brightness value
     *
     * @return int
     */
    int getBrightness() const;

    /**
     * set brightness value
     *
     * @param value
     */
    void setBrightness(int value);

    /**
     * get a range of supported contrast values
     *
     * @return Range : contrast range
     */
    Range getSupportedContrast() const;


    /**
     *get current contrast tone value
     *
     *(HAL3 only)
     *
     *@return Tonemap_RBG
     */
    Tonemap_RBG getContrastTone() const;

    /**
     * get current contrast value
     *
     * @return int
     */
    int getContrast() const;

    /**
     * set contrast tone value
     *
     * (HAL3 only)
     *
     * @param value
     */
    void setContrastTone(Tonemap_RBG& tonemap);

    /**
     * set contrast value
     *
     * @param value
     */
    void setContrast(int value);

    /**
     * get supported ranges for preview FPS The FPS range has valid
     * min and max value. Actual fixed point FPS value is calculated
     * by dividing the min and max values by 1000. For example, max
     * value of 26123 represents 26.123 fps.
     *
     * @return vector<Range> : preview fps ranges
     */
    std::vector<Range> getSupportedPreviewFpsRanges() const;

    /**
     * get current preview fps range value
     *
     * @return Range
     */
    Range getPreviewFpsRange() const;

    /**
     * set preview fps range value
     *
     * @param value
     */
    void setPreviewFpsRange(const Range& value);

    /**
     * get a list of fixed FPS values for video stream.
     *
     * @return vector<VideoFPS> : video fps values
     */
    std::vector<VideoFPS> getSupportedVideoFps() const;

    /**
     * get current video fps mode
     *
     * @return VideoFPS
     */
    VideoFPS getVideoFPS() const;

    /**
     * set video fps mode.
     *
     * Note: Setting the mode to high framerate will override
     * preview FPS settings. see \ref VideoFPS for high framerate
     * values.
     *
     * @param VideoFPS
     */
    void setVideoFPS(VideoFPS value);

    /**
     * get a list of supported preview formats
     *
     * @return std::vector<std::string>
     */
    std::vector<std::string> getSupportedPreviewFormats() const;

    /**
     * get current preview format
     *
     * @return std::string
     */
    std::string getPreviewFormat() const;

    /**
     * set preview format
     *
     * @param value
     */
    void setPreviewFormat(const std::string& value);

     /**
     * set picture format (HAL3 only)
     *
     * @param value
     */
     void setPictureFormat(const std::string& value);

     /**
     * set manual exposure in sensor
     *
     * @param value
     */
    void setManualExposure(int value);

    /**
     * set manual gain in sensor
     *
     * @param value
     */
    void setManualGain(int value);

    /**
     * get range of manual exposure value accepted by by sensor
     *
     * @param size Image Size
     & @param fps*
     *
     * @return Range : Min and Max values
     **/
    Range64 getManualExposureRange(ImageSize size, int fps);

    /**
     * get range of manual gain value accepted by by sensor
     *
     * @param size Image Size
     & @param fps*
     *
     * @return Range : Min and Max values
     */
    Range getManualGainRange(ImageSize size, int fps);
    /**
     * get manual exposure time
     *
     * @param value
     */
    int getExposureTime() const;
    /**
     * set manual exposure time
     *
     * @param value
     */
    void setExposureTime(const std::string& value);
    /**
     * set vertical flip bit in sensor
     *
     * @param value
     */
    void setVerticalFlip(bool value);

    /**
     * set horizontal mirror bit in sensor
     *
     * @param value
     */
    void setHorizontalMirror(bool value);


     /**
     * set stats logging mask
     *
     * @param value
     */
    void setStatsLoggingMask(int value);

    /**
    * set anti-banding
    *
    * @param value
    */
    void setAntibanding(const std::string& value);

    /**
     * Get exposure time (ns) of the requested frame (HAL3 only)
     *
     * @param value
	 * @return Frame exposure time in nSec
     */
    uint64_t getFrameExposureTime(ICameraFrame* frame);

    /**
     * Get gain settings of the requested frame (HAL3 only)
     *
	 * @param value
     */
    int32_t getFrameGainValue(ICameraFrame* frame); 

	/**
	 * Get rolling shutter skew settings of the requested frame
	 * (HAL3 only)
     *
	 * @param value
	 * @return Rolling shutter skew duration in nSec
     */
	uint64_t getFrameRollingShutterSkew(ICameraFrame* frame);

	 /**
	 * Get readout timestamp of the requested frame (HAL3 only)
     *
          *@param value
          *@return Readout timestamp in nSec
     */
	uint64_t getFrameReadoutTimestamp(ICameraFrame* frame);


     /**
	 * Get readout duration of the requested frame (HAL3 only)
     *
	 * @param value
	 * @return Readout duration in nSecs
     */
	uint64_t getFrameReadoutDuration(ICameraFrame* frame);

private:
#if defined(_HAL3_CAMERA_)
    std::vector<Range> getHFRFpsRange() const;
#endif
    /**
     * private implementation and storage handle for parameters
     */
    void *priv_;

    /**
     * handle to attached camera device
     */
    ICameraDevice *device_;
}; /* class CameraParams */

} /* namespace camera */

#endif  /* __CAMERA_PARAMETERS_H__ */

