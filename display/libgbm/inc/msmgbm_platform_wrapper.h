//**************************************************************************************************
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//**************************************************************************************************

#ifndef __MSMGBM_PFM_WRP_H__
#define __MSMGBM_PFM_WRP_H__

#include "msmgbm_adreno_utils.h"
#include "gbm_priv.h"
#include "msmgbm.h"

#ifdef VENUS_COLOR_FORMAT
#include <media/msm_media_info.h>
#else
#define VENUS_Y_STRIDE(args...) 0
#define VENUS_Y_SCANLINES(args...) 0
#define VENUS_BUFFER_SIZE(args...) 0
#endif

namespace msm_gbm {
/**
 * Function to query if CPU access usage flags are set
 * @return    : true/false
 *
 */
bool cpu_can_accss(int prod_usage, int cons_usage);
/**
 * Helper function to query if CPU access usage flags are set
 * @return    : true/false
 *
 */
bool cpu_can_rd(int prod_usage, int cons_usage);
/**
 * Helper function to query if CPU access usage flags are set
 * @return    : true/false
 *
 */
bool cpu_can_wr(int prod_usage);
/**
 * Function to query bpp depending on format
 * @return    : bpp value
 *
 */
uint32_t get_bpp_for_uncmprsd_rgb_fmt(int format) ;

class platform_wrap {
 public:
  platform_wrap();
  ~platform_wrap();

  /**
   * Function to Initialize the platform wrapper object
   * @return   true/
   *           false
   */
  bool init(void);
  /**
   * Function to query UBWC support
   * @return   true/
   *           false
   */
  bool is_ubwc_enbld(int format, int prod_usage, int cons_usage);
  /**
   * Function to query MacroTile support
   * @return   true/
   *           false
   */
  bool is_mcro_tile_enbld(int format, int prod_usage, int cons_usage);
  /**
   * Function to query the UBWC Enabled Buffer Size
   * @return   size on success
   *           0 on fail
   */
  unsigned int get_ubwc_size(int width, int height, int format, unsigned int alignedw, unsigned int alignedh);
  /**
   * Helper Function to query the UBWC Enabled Buffer Size
   * @return   size on success
   *           0 : fail
   */
  unsigned int get_rgb_ubwc_mb_size(int width, int height, uint32_t bpp);
  /**
   * Helper Function to query the UBWC Enabled Buffer Size
   * @return   block width and height
   */
  void get_rgb_ubwc_blk_size(uint32_t bpp, int *block_width, int *block_height);
  /**
   * Function to query the RGB format support
   * @return   true : success
   *           false : fail
   */
  int is_valid_rgb_fmt(int gbm_format);

  uint32_t get_bpp_for_uncmprsd_rgb_fmt(int format);

  /**
   * Function to check whether the format is uncompressed RGB
   * @params    gbm format
   * @return    boolean 0 (compressed RGB format)
   *                    1 (uncompressed RGB format)
   *
   */
  bool is_valid_uncmprsd_rgb_fmt(int format);

  /**
   * Function to query UBWC feature support
   * @return   true : success
   *           false : fail
   */
  bool is_ubwc_support_enbld(int format);
  /**
   * Function to query UBWC enabled format support
   * @return   true : success
   *           false : fail
   */
  bool is_valid_ubwc_fmt(int format);
  /**
   * Function to get  aligned width and height depending on the underlying GPU/Video platform
   * @return    aligned_w
   *            aligned_h
   */
  void get_aligned_wdth_hght(gbm_bufdesc *descriptor, unsigned int *aligned_w,
                                      unsigned int *aligned_h);

  /**
   * Function to get  size aligned width and height depending on the underlying GPU/Video platform
   * @params    gbm format
   *            width of the buffer
   *            height of the buffer
   *            usage flags
   *            aligned height
   *            aligned weight
   * @return    size of the buffer
   */
   unsigned int get_size(int format, int width, int height, int usage,
                             int alignedw, int alignedh);

  /**
   * Function to get  aligned width and height for YUV format
   * @params    gbm format
   *            width of the buffer
   *            height of the buffer
   * @return    aligned height
   *            aligned weight
   *
   */
    void get_yuv_ubwc_wdth_hght(int width, int height, int format,
                                          unsigned int *aligned_w, unsigned int *aligned_h);

 private:
  bool gpu_support_macrotile = false;
  bool display_support_macrotile = false;
  adreno_mem_info *adreno_helper_ = NULL;

};

extern "C" {
    /**
    * C wrapper Function to get  aligned width and height depending on the underlying GPU/Video platform
    * @return    aligned_w
    *            aligned_h
    */
    void qry_aligned_wdth_hght(gbm_bufdesc *descriptor, unsigned int *alignedw, unsigned int *alignedh);

    /**
     * C wrapper Function to query size based on format from the platform wrapper
     * @return    : size
     *
     */
    unsigned int qry_size(gbm_bufdesc *desc, unsigned int alignedw, unsigned int alignedh);

    /**
    * C wrapper Function to check whether the format is UBWC or not.
    * @params    gbm format
    * @return    boolean 0 (non UBWC format)
    *                    1 (UBWC format)
    */
    bool is_valid_ubwc_format(int format);

    /**
     * Function to return bytes per pixel for a given uncompressed RGB format
     * @params    uncompressed RGB gbm format
     * @return    bytes per pixel
     *
     */
    uint32_t get_bpp_for_uncmprsd_rgb_format(int format);

    /**
     * Function to return bytes needed to represent UBWC RGB Metabuffer
     * @params    RGB gbm format
     * @return    size of UBWC RGB meta buffer
     *
     */
    uint32_t get_rgb_ubwc_metabuffer_size(int width, int height, int bpp);

    /**
    * C wrapper Function to check whether the format is uncompressed RGB format or not.
    * @params    gbm format
    * @return    boolean 0 (compressed RGB format)
    *                    1 (uncompressed RGB format)
    */
    bool is_valid_uncmprsd_rgb_format(int format);

    /**
     * C wrapper Function to delete platform wrapper object
     * @return  : None
     *
     */
    void platform_wrap_deinstnce(void);

    /**
     * C wrapper Function to create a cpp object of platform wrapper class
     * @return   0 : success
     *           1 : failure
     */
    bool platform_wrap_instnce(void);

}

}  // namespace msm_gbm

#endif  // __MSMGBM_PFM_WRP_H__
