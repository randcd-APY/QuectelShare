/**********************************************************************
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#ifndef __QCAMERA_PPDGCORE_H__
#define __QCAMERA_PPDGCORE_H__

extern "C" {
#include "img_common.h"
#include "img_meta.h"
#include "img_queue.h"
}

#include "DG/API/mvDGIP.h"

#include "img_dbg.h"

#define TAG "CameraPPDgCore: "

#define DGDBG_INFO(fmt, args...)    IDBG_INFO(TAG fmt, ##args)
#define DGDBG_ERROR(fmt, args...)   IDBG_ERROR(TAG fmt, ##args)
#define DGDBG_HIGH(fmt, args...)    IDBG_HIGH(TAG fmt, ##args)
#define DGDBG_MED(fmt, args...)     IDBG_MED(TAG fmt, ##args)
#define DGDBG_LOW(fmt, args...)     IDBG_LOW(TAG fmt, ##args)


/**
*  MACROS and CONSTANTS
 **/
#define INPUT_FRAME_IDX                 0
#define REFERENCE_FRAME_IDX             1
#define OUTPUT_FRAME_IDX                0
#define MAX_FRAMES_FOR_DUMP             30
#define API_ERROR                       -1
#define FASTRPC_ERROR                   -2

typedef struct
{
  int    winW;
  int    winH;
  int    rBits;
  int    gBits;
  int    bBits;
  int    aBits;
  int    bitsPerPixel;
  int    depthBits;
  int    stencilBits;
  int    sampleBuffers;
  int    samples;
}Config;

/*
 * ppeis context Data
 */

typedef struct _ppeis_context_t{
  void *user_data;
  mvDGCameraFrameContentInfo outFrame;
}ppdg_context_t;


/*========QCameraPPDgCore====================================================
 * Class: QCameraPPDgCore
 *
 * Description: This class represents a temple implementation
 *
 * Notes: none
 *==========================================================================*/
class QCameraPPDgCore
{
public:
 /**
  * Function: QCameraPPDgCore
  *
  * Description: constructor of class
  *
  * Arguments:
  *   none
  *
  * Return values:
  *     none
  *
  * Notes: none
  **/
  QCameraPPDgCore();

 /**
  * Function: ~QCameraPPDgCore
  *
  * Description: destructor of class
  *
  * Arguments:
  *   none
  *
  * Return values:
  *     none
  *
  * Notes: none
  **/
  ~QCameraPPDgCore();

 /**
  * Function: Init
  *
  * Description: Initialization for the class
  *
  * Arguments:
  *   none
  *
  * Return values:
  *     True/False: Status of  initialization
  *
  * Notes: none
  **/
bool Init();

 /**
  * Function: DeInit
  *
  * Description: Deinitialization for the class
  *
  * Arguments:
  *   none
  *
  * Return values:
  *     True/False: Status of  initialization
  *
  * Notes: none
  **/

bool DeInit();

void getDGConfig();

bool startSession();

bool stopSession();

bool IsDeinitStarted() {
  return bIsDeinitStarted;
}

void Flush();

 /**
  * Function: processData
  *
  * Description: main processing body
  *
  * Arguments:
  *   @arg1: p_in_frame: input frame handler
  *   @arg2: in_frame_cnt: number of input frames
  *   @arg3: p_out_frame: output frame handler
  *   @arg4: out_frame_cnt: number of output frames
  *   @arg3: p_meta: frame meta data, containing ROI info
  *
  * Return values:
  *   IMG_SUCCESS or IMG_ERR_GENERAL
  *
  * Notes: none
  **/
  int8_t processData(img_frame_t *inFrame[], int inFrameCnt,
    img_frame_t *outFrame[], int outFrameCnt,
    img_meta_t* p_meta);

 /**
  * Function: setFrameOps
  *
  * Description: set frame ops
  *
  * Arguments:
  *   @pFrameOps: pointer to frame ops
  *
  * Return values:
  *   none
  *
  * Notes: none
  **/
  inline void setFrameOps(img_frame_ops_t *pFrameOps) {mFrameOps = *pFrameOps;}

  inline img_matrix_type_t getSessionType() {return mMatrixType;}

  void setMatrix(img_matrix_type_t matrixType);

  void setInputSizes(img_dim_t *img_dim);

  void setOutputSizes(img_dim_t *img_dim);

  int fillFuncTable(void);

private:

 /** bufInfo_t
  *  @addr:    buf addr
  *  @length:  buf length
  *  @fd:      buf FD
  *
  *  Struct for registered buf list
  */
  typedef struct {
    uint8_t* addr;
    uint32_t length;
    int32_t fd;
  } bufInfo_t;

  /*
   * Thread context Data
   */
  typedef struct _ppeis_thread_data_t {
    int32_t frameId;

  } ppdg_thread_data_t;

  typedef struct {
    mvDGIP* (*init_get_handle)();
    void (*deinit)(mvDGIP*);
    int32_t (*start_session)(mvDGIP*,mvDGConfiguration*);
    int32_t (*stop_session)(mvDGIP*, int32_t*);
    void (*set_frame_transf_matrix)(mvDGIP* pObj,
                  mvDGTransfMatrixArrayInfo* pTransfArray);
    void (*set_frame_content)(mvDGIP* pObj,
                  mvDGCameraFrameContentInfo* pImgDataInfo);
    int32_t (*get_output_image)(mvDGIP* pObj, int32_t frameid,
                  mvDGCameraFrameContentInfo* pOutImgInfo, mvDGROI *pROI);
    const char *(*get_version)(void);
  } lib_funct_table;

  /*
   * Default context Data
   */


 /**
  * Frame ops
  */
  img_frame_ops_t mFrameOps;

 /**
  * Buffer count
  */
  int32_t mBufCnt;

 /**
  * flag to check if module is initialized and avoid multiple initialization
  */
  bool bIsDeinitStarted;
  img_matrix_type_t mMatrixType;

  mvDGConfiguration   mvDGConfig;

  bool bIsInputSizeSet;
  bool bIsOutputSizeSet;

  bool bDgP2LEnabled;

public:
//  Config              cfg;
  int (*callback)(void *user_data);
  img_queue_t         callBackQ;
  pthread_t           threadId;
  ppdg_thread_data_t  *m_pThreadData;
//  ppdg_context_t      *m_pPpdgContext;
  mvDGIP                *pLibHdl;
  void                *pDgipLibFd;
  lib_funct_table     funct_pointers;

};

#endif
