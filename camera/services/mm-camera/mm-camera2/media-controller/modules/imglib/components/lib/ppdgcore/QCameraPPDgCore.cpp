/**********************************************************************
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "QCameraPPDgCore.h"
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <assert.h>
#include <sys/time.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <android/log.h>
#include <vector>

#ifdef _ANDROID_
#include <utils/Log.h>
#include <cutils/properties.h>
#endif
#include "img_dbg.h"

#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1080
#define DUMP_2DWARP 0

static void ScaleAndCenter(float *matrix, int num_vertices, int width,
  int height, int centerX, int centerY, float width_ratio, float height_ratio)
{
  float w, h;
  float w1, h1, w2, h2;
  float w_adj, h_adj;


  w = (float) width;
  h = (float) height;
  w1 = 0.5f*width_ratio;
  h1 = 0.5f*height_ratio;
  w_adj = 0.00001f;
  h_adj = 0.00001f;
  float centerAdjustmentX = w1*(1.0f+ 1.0f/w) - w_adj;
  float centerAdjustmentY = h1*(1.0f + 1/h) - h_adj;

  float finalCenterAdjustmentX = (float)centerX/w;
  float finalCenterAdjustmentY = (float)centerY/h;

  int stride = num_vertices*3;
  for(int i=0; i<num_vertices; i++)
  {
    matrix[1] *= h/w;
    matrix[2] = matrix[2]/w;
    w2 = (float) (matrix[0]*centerAdjustmentX +
                  matrix[1]*centerAdjustmentY);
    matrix[2] -= w2;


    matrix[stride+0] *= w/h;
    matrix[stride+2] = matrix[stride+2]/h;
    h2 = (float) (matrix[stride+0]*centerAdjustmentX +
                  matrix[stride+1]*centerAdjustmentY);
    matrix[stride+2] -= h2;


    matrix[2*stride+0] *= w;
    matrix[2*stride+1] *= h;
    h2 = matrix[2*stride+0]*centerAdjustmentX+
      matrix[2*stride+1]*centerAdjustmentY;
    matrix[2*stride+2] -= h2;

    matrix[0] += finalCenterAdjustmentX*matrix[2*stride+0];
    matrix[1] += finalCenterAdjustmentX*matrix[2*stride+1];
    matrix[2] += finalCenterAdjustmentX*matrix[2*stride+2];
    matrix[stride+0] += finalCenterAdjustmentY*matrix[2*stride+0];
    matrix[stride+1] += finalCenterAdjustmentY*matrix[2*stride+1];
    matrix[stride+2] += finalCenterAdjustmentY*matrix[2*stride+2];

    /* print out matrix for testing purpose
    printf(
    "======================== start ============================\n"
    "row 1: %10.9f,  %10.9f  %10.9f\n"
    "row 2: %10.9f,  %10.9f  %10.9f\n"
    "row 3: %10.9f,  %10.9f  %10.9f\n"
    "======================== end ============================\n",
    matrix[0], matrix[1], matrix[2],
    matrix[stride+0], matrix[stride+1], matrix[stride+2],
    matrix[2*stride+0], matrix[2*stride+1], matrix[2*stride+2]); */

    matrix+=3;
  }
}

static void getTransformationMatrices(float* matrixIn, float* matrixOut,
  int num_vertices, int texWidth, int texHeight, int centerX, int centerY,
  float width_ratio, float height_ratio)
{
  DGDBG_LOW("num_vertices %d ", num_vertices);


  DGDBG_LOW("Matrix 1 : %f %f %f | %f %f %f | %f %f %f ",
    matrixIn[0], matrixIn[1],matrixIn[2],
    matrixIn[3], matrixIn[4],matrixIn[5],
    matrixIn[6], matrixIn[7],matrixIn[8]);
  DGDBG_LOW("Matrix 2 : %f %f %f | %f %f %f | %f %f %f ",
    matrixIn[9], matrixIn[10],matrixIn[11],
    matrixIn[12], matrixIn[13],matrixIn[14],
    matrixIn[15], matrixIn[16],matrixIn[17]);

  for (int i = 0,j = 0; i < num_vertices >> 1; i++, j+=9)
  {
#if 1 //COL_MAJOR
    matrixOut[i * 6 + 0] = matrixIn[j +0];
    matrixOut[i * 6 + 1] = matrixIn[j +1];
    matrixOut[i * 6 + 2] = matrixIn[j +2];

    matrixOut[i * 6 + 3] = matrixIn[j +0];
    matrixOut[i * 6 + 4] = matrixIn[j +1];
    matrixOut[i * 6 + 5] = matrixIn[j +2];

    matrixOut[num_vertices *3 + i * 6 + 0] = matrixIn[j +3];
    matrixOut[num_vertices *3 + i * 6 + 1] = matrixIn[j +4];
    matrixOut[num_vertices *3 + i * 6 + 2] = matrixIn[j +5];

    matrixOut[num_vertices *3 + i * 6 + 3] = matrixIn[j +3];
    matrixOut[num_vertices *3 + i * 6 + 4] = matrixIn[j +4];
    matrixOut[num_vertices *3 + i * 6 + 5] = matrixIn[j +5];

    matrixOut[num_vertices *6 + i * 6 + 0] = matrixIn[j +6];
    matrixOut[num_vertices *6 + i * 6 + 1] = matrixIn[j +7];
    matrixOut[num_vertices *6 + i * 6 + 2] = matrixIn[j +8];

    matrixOut[num_vertices *6 + i * 6 + 3] = matrixIn[j +6];
    matrixOut[num_vertices *6 + i * 6 + 4] = matrixIn[j +7];
    matrixOut[num_vertices *6 + i * 6 + 5] = matrixIn[j +8];
#endif
#ifdef ROW_MAJOR
    matrixOut[i * 6 + 0] = matrixIn[j +0];
    matrixOut[i * 6 + 1] = matrixIn[j +3];
    matrixOut[i * 6 + 2] = matrixIn[j +6];

    matrixOut[i * 6 + 3] = matrixIn[j +0];
    matrixOut[i * 6 + 4] = matrixIn[j +3];
    matrixOut[i * 6 + 5] = matrixIn[j +6];

    matrixOut[num_vertices *3 + i * 6 + 0] = matrixIn[j +1];
    matrixOut[num_vertices *3 + i * 6 + 1] = matrixIn[j +4];
    matrixOut[num_vertices *3 + i * 6 + 2] = matrixIn[j +7];

    matrixOut[num_vertices *3 + i * 6 + 3] = matrixIn[j +1];
    matrixOut[num_vertices *3 + i * 6 + 4] = matrixIn[j +4];
    matrixOut[num_vertices *3 + i * 6 + 5] = matrixIn[j +7];

    matrixOut[num_vertices *6 + i * 6 + 0] = matrixIn[j +2];
    matrixOut[num_vertices *6 + i * 6 + 1] = matrixIn[j +5];
    matrixOut[num_vertices *6 + i * 6 + 2] = matrixIn[j +8];

    matrixOut[num_vertices *6 + i * 6 + 3] = matrixIn[j +2];
    matrixOut[num_vertices *6 + i * 6 + 4] = matrixIn[j +5];
    matrixOut[num_vertices *6 + i * 6 + 5] = matrixIn[j +8];
#endif
  }

  for(int k = 0 ; k < 25 ; k+=12) {
    DGDBG_LOW("Matrix col %d : %f %f %f | %f %f %f | %f %f %f | %f %f %f", k,
      matrixOut[k+0], matrixOut[k+1],matrixOut[k+2],
      matrixOut[k+3], matrixOut[k+4],matrixOut[k+5],
      matrixOut[k+6], matrixOut[k+7],matrixOut[k+8],
      matrixOut[k+9], matrixOut[k+10],matrixOut[k+11]);
  }

  ScaleAndCenter(matrixOut, num_vertices, texWidth, texHeight, centerX,
  centerY, width_ratio, height_ratio);
}


static void getTransformationMatrices2Dwarp(float* matrixIn, float* matrixOut,
  int num_vertices, int texWidth, int texHeight, int centerX, int centerY,
  float width_ratio, float height_ratio)
{
  DGDBG_LOW("num_vertices %d ", num_vertices);

#if DUMP_2DWARP
  static int matrix_log = 2;
  matrix_log++;
  FILE* file;
  char filename[128];
#endif


  DGDBG_MED("Matrix 1 : %f %f %f | %f %f %f | %f %f %f ",
    matrixIn[0], matrixIn[1], matrixIn[2],
    matrixIn[3], matrixIn[4], matrixIn[5],
    matrixIn[6], matrixIn[7], matrixIn[8]);
  DGDBG_MED("Matrix 2 : %f %f %f | %f %f %f | %f %f %f ",
    matrixIn[9], matrixIn[10], matrixIn[11],
    matrixIn[12], matrixIn[13], matrixIn[14],
    matrixIn[15], matrixIn[16], matrixIn[17]);

  for (int i = 0, j = 0; i < num_vertices; i++, j += 9) {
    matrixOut[i * 3 + 0] = matrixIn[j + 0];
    matrixOut[i * 3 + 1] = matrixIn[j + 1];
    matrixOut[i * 3 + 2] = matrixIn[j + 2];

    matrixOut[num_vertices * 3 + i * 3 + 0] = matrixIn[j + 3];
    matrixOut[num_vertices * 3 + i * 3 + 1] = matrixIn[j + 4];
    matrixOut[num_vertices * 3 + i * 3 + 2] = matrixIn[j + 5];

    matrixOut[num_vertices * 6 + i * 3 + 0] = matrixIn[j + 6];
    matrixOut[num_vertices * 6 + i * 3 + 1] = matrixIn[j + 7];
    matrixOut[num_vertices * 6 + i * 3 + 2] = matrixIn[j + 8];
  }

  for (int k = 0 ; k < 25 ; k += 12) {
    DGDBG_MED("Matrix col %d : %f %f %f | %f %f %f | %f %f %f | %f %f %f", k,
      matrixOut[k + 0], matrixOut[k + 1], matrixOut[k + 2],
      matrixOut[k + 3], matrixOut[k + 4], matrixOut[k + 5],
      matrixOut[k + 6], matrixOut[k + 7], matrixOut[k + 8],
      matrixOut[k + 9], matrixOut[k + 10], matrixOut[k + 11]);
  }

  ScaleAndCenter(matrixOut, num_vertices, texWidth, texHeight, centerX,
  centerY, width_ratio, height_ratio);

#if DUMP_2DWARP
  if ( matrix_log >= 5 && matrix_log < 15) {
    sprintf(filename, "/data/misc/camera/Scalencentre_%d.txt",matrix_log);
    file = fopen(filename,"w");
    if (!file) {
      for( int i =0 ; i < 3 ; i++) {
        for(int j =0; j< num_vertices; j++) {
          for(int k =0; k<3; k++) {
            fprintf(file,"%f ",matrixOut[i*num_vertices + j*3 + k] );
          }
          fprintf(file, "\n");
        }
        fprintf(file, "\n\n\n");
      }
    } else {
      DGDBG_MED("Error in file opening");
    }
  }
#endif
}

int ppdg_can_wait(void *p_userdata)
{
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore*)p_userdata;
  if(!pCamPPDgCore) {
    return 0;
  }

  if (pCamPPDgCore->IsDeinitStarted()) {
    DGDBG_MED(": ppdg_can_wait: CamPPEisCore Deinit started");
    return 0;
  }

  return 1;
}

void *ppdg_wait_thread_loop(void *handle)
{
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore*)handle;
  img_msg_t *p_msg = NULL;
  ppdg_context_t *m_pPpdgContext = NULL;
  mvDGROI dgRoi;
  int32_t status;

  while (TRUE) {
    DGDBG_LOW(": E img_q_wait, callBackQ size %d",
              img_q_count(&pCamPPDgCore->callBackQ));
    m_pPpdgContext = (ppdg_context_t *)
      img_q_wait(&pCamPPDgCore->callBackQ, ppdg_can_wait, pCamPPDgCore);

    if (!ppdg_can_wait(pCamPPDgCore)) {
      break;
    }
    else if (!m_pPpdgContext) {
      continue;
    }

    DGDBG_LOW("img_q_wait done");

    if (!pCamPPDgCore->callback ||
      !m_pPpdgContext->user_data) {
      DGDBG_ERROR("invalid input arguments %p %p", pCamPPDgCore->callback,
        m_pPpdgContext->user_data);
      break;
    } else {
//      DGDBG_INFO("get out image: frameid: %d, addr: %p, ionfd: %d",
//                 m_pPpdgContext->outFrame.frameid,
//                 m_pPpdgContext->outFrame.pxlNV12,
//                 m_pPpdgContext->outFrame.ionFd);
//      status = pCamPPDgCore->funct_pointers.get_output_image(
//            pCamPPDgCore->pLibHdl,
//            m_pPpdgContext->outFrame.frameid,
//            &m_pPpdgContext->outFrame, &dgRoi);
//      if (status) {
//        DGDBG_ERROR("Failed to get output image: status: %d", status);
//      }
      DGDBG_INFO("done");
      DGDBG_LOW("user_data %p %p", pCamPPDgCore->callback,
        m_pPpdgContext->user_data);
      pCamPPDgCore->callback(m_pPpdgContext->user_data);
      free(m_pPpdgContext);
    }
    DGDBG_LOW(": X");
  }
  DGDBG_LOW("Exiting thread");
  return NULL;
}

/**
 * Function: img_algo_init
 *
 * Description: algorirhm initialization
 *
 * Arguments:
 *   @pp_context: return context [Output parameter]
 *   @p_params: init params
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
extern "C" int img_algo_init(void **p_context, img_init_params_t *p_params)
{
  DGDBG_ERROR(": PPDgCore: E");
  int ret = IMG_ERR_GENERAL;
  IMG_UNUSED(p_params);
  QCameraPPDgCore *pCamPPDgCore = new QCameraPPDgCore;

  if (pCamPPDgCore) {
    *p_context = pCamPPDgCore;
    if (pCamPPDgCore->fillFuncTable()) {
      if (pCamPPDgCore->Init()) {
        img_q_init(&pCamPPDgCore->callBackQ, (char *)"PPEISFrameCbQ");
        ret = IMG_SUCCESS;
      } else {
        DGDBG_ERROR("QCameraPPDgCore Init failed!");
      }
    } else {
      ret = IMG_ERR_NOT_FOUND;
    }
  }

  DGDBG_INFO(": X");
  return ret;
}

/**
 * Function: img_algo_deinit
 *
 * Description: algorithm de-initialization
 *
 * Arguments:
 *   @arg1:
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
extern "C" int img_algo_deinit(void *p_context)
{
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore*)p_context;

  DGDBG_INFO("img_algo_deinit called");
  assert(NULL != p_context);

  if (pCamPPDgCore) {
    (void)pCamPPDgCore->DeInit();
    img_q_deinit(&pCamPPDgCore->callBackQ);
    delete pCamPPDgCore;
    pCamPPDgCore = NULL;
  }

  DGDBG_LOW(": X");
  return IMG_SUCCESS;
}

/**
 * Function: img_algo_start
 *
 * Description: algorithm start
 *
 * Arguments:
 *   @arg1:
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
extern "C" int img_algo_start(void *p_context)
{
  int ret = IMG_SUCCESS;
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore*)p_context;

  DGDBG_LOW("E");
  assert(NULL != p_context);

//  if (false == pCamPPDgCore->startSession()) {
//    ret = IMG_ERR_GENERAL;
//  }

  DGDBG_LOW(": X");
  return ret;
}

/**
 * Function: img_algo_stop
 *
 * Description: algorithm stop
 *
 * Arguments:
 *   @arg1:
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
extern "C" int img_algo_stop(void *p_context)
{
  int ret = IMG_SUCCESS;
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore*)p_context;

  DGDBG_INFO("E");
  assert(NULL != p_context);

  if (false == pCamPPDgCore->stopSession()) {
    ret = IMG_ERR_GENERAL;
  }

  DGDBG_INFO(": X");
  return ret;
}

/**
 * Function: img_algo_set_callback
 *
 * Description: Set callback pointer
 *
 * Arguments:
 *   @p_context: class pointer
 *   @algo_callback: user callback function
 *
 * Return values:
 *   IMG_SUCCESS or IMG_ERR_INVALID_INPUT
 *
 * Notes: none
 **/
extern "C" int img_algo_set_callback(void *p_context,
  int (*algo_callback)(void *user_data))
{
  assert(NULL != p_context);
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore *)p_context;
  int ret = IMG_SUCCESS;

  DGDBG_LOW("E");

  if (!pCamPPDgCore || !algo_callback) {
    DGDBG_ERROR(": failed: %p %p", pCamPPDgCore, algo_callback);
    return IMG_ERR_INVALID_INPUT;
  }

  pCamPPDgCore->callback = algo_callback;
  int retval = pthread_create(&pCamPPDgCore->threadId, NULL,
    ppdg_wait_thread_loop, (void*)p_context);
  pthread_setname_np(pCamPPDgCore->threadId, "PPDG_wait_thread");
  if (retval < 0) {
    DGDBG_ERROR("PPDG_wait_thread creation failed");
    return IMG_ERR_GENERAL;
  } else {
    DGDBG_MED("PPDG_wait_thread creation success");
  }

  DGDBG_LOW("X");

  return IMG_SUCCESS;
}

/**
 * Function: img_algo_process
 *
 * Description: algorithm main body
 *
 * Arguments:
 *   @arg1: p_context: class pointer
 *   @arg2: p_in_frame: input frame handler
 *   @arg3: in_frame_cnt: number of input frames
 *   @arg4: p_out_frame: output frame handler
 *   @arg5: out_frame_cnt: number of output frames
 *   @arg6: p_meta: frame meta data
 *   @arg7: meta_cnt: number of meta data array
 *
 * Return values:
 *   IMG_SUCCESS or IMG_ERR_GENERAL
 *
 * Notes: none
 **/
extern "C" int img_algo_process(void *p_context, img_frame_t *p_in_frame[],
  int in_frame_cnt,img_frame_t *p_out_frame[],
  int out_frame_cnt, img_meta_t *p_meta[], int meta_cnt)
{
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore *)p_context;

  DGDBG_LOW("frames count - in: %d, out: %d", in_frame_cnt, out_frame_cnt);

  int ret = IMG_SUCCESS;
  if (p_context != NULL && p_meta != NULL) {
  } else {
    DGDBG_ERROR(" img_algo_process failed");
    ret = IMG_ERR_GENERAL;
  }
  ret = pCamPPDgCore->processData(p_in_frame, in_frame_cnt, p_out_frame,
    out_frame_cnt,(p_meta && (meta_cnt > 0)) ? p_meta[0] : NULL);

  DGDBG_LOW(": X - ret: %d", ret);

  return ret;
}

/**
 * Function: img_algo_set_frame_ops
 *
 * Description: Function to set frame operations
 *
 * Arguments:
 *   @p_context: ptr to instance
 *   @p_ops: ptr to frame ops
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
extern "C" int img_algo_set_frame_ops(void *p_context,
  img_frame_ops_t *p_ops)
{
  DGDBG_LOW(": E");
  int ret = IMG_SUCCESS;
  assert(NULL != p_context);
  assert(NULL != p_ops);

  QCameraPPDgCore* instance = (QCameraPPDgCore*) p_context;
  instance->setFrameOps(p_ops);

  DGDBG_LOW(": X");
  return ret;
}

/**
 * Function: img_algo_set_lib_config
 *
 * Description: Set algo config params
 *
 * Arguments:
 *   @p_context: class pointer
 *   @user_data: Payload data to be set to algo
 *
 * Returns:
 *   IMGLIB return codes
 *
 * Notes: none
 **/
extern "C" int img_algo_set_lib_config(void *p_context, void* user_data)
{

  int ret = IMG_SUCCESS;
  img_matrix_type_t matrixType = MATRIX_NONE;
  img_lib_config_t *p_lib_config = (img_lib_config_t *)user_data;

  if(!p_lib_config || !p_lib_config->lib_data || !p_context) {
    return IMG_ERR_INVALID_INPUT;
  }

  img_lib_param_t  lib_param = p_lib_config->lib_param;
  QCameraPPDgCore *pCamPPDgCore = (QCameraPPDgCore *)p_context;

  switch(lib_param) {
  case IMG_ALGO_PRESTART:
    break;
  case IMG_ALGO_IN_FRAME_DIM: {
    img_dim_t *dims = (img_dim_t *)p_lib_config->lib_data;
    pCamPPDgCore->setInputSizes(dims);
    free(dims);
  }
    break;
  case IMG_ALGO_OUT_FRAME_DIM: {
    img_dim_t *out_dim = (img_dim_t *)p_lib_config->lib_data;
    pCamPPDgCore->setOutputSizes(out_dim);
    free(out_dim);
  }
    break;
  default:
    ret = IMG_ERR_INVALID_INPUT;
    break;
  }

  return ret;

}

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
QCameraPPDgCore::QCameraPPDgCore()
{
  DGDBG_LOW(": E");

  memset(&mFrameOps, 0x0, sizeof(mFrameOps));
  mBufCnt = 0;
  m_pThreadData = NULL;
//  cfg = { DEFAULT_WIDTH, DEFAULT_HEIGHT, 8, 8, 8, 8, 32, 24, 8, 0, 0 };
  memset(&m_pThreadData, 0x0, sizeof(m_pThreadData));
  DGDBG_LOW(": X");
}

/**
 * Function: ~QCameraPPDgCore
 *
 * Description: destructor of class
 *
 * Arguments:
 *   @arg1:
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
QCameraPPDgCore::~QCameraPPDgCore()
{

}

/**
 * Function: QCameraPPDgCore: Init
 *
 * Description: Init of class
 *
 * Arguments:
 *   @arg1:
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
bool QCameraPPDgCore::Init()
{
  bool ret = false;
  bIsDeinitStarted = false;

  DGDBG_LOW("E");

  m_pThreadData = new (ppdg_thread_data_t);

  if ((m_pThreadData != NULL) && (funct_pointers.init_get_handle)) {
    pLibHdl = funct_pointers.init_get_handle();
    if (!pLibHdl) {
      DGDBG_ERROR("Initialize DGIP library Failed!");
    } else {
      DGDBG_INFO("DG IP library handle: %p", pLibHdl);
    }
    DGDBG_INFO("DG IP library version is: %s", funct_pointers.get_version());
    ret = true;
  }
  bIsInputSizeSet = false;
  bIsOutputSizeSet = false;

  memset(&mvDGConfig, 0, sizeof(mvDGConfiguration));

  DGDBG_LOW("X");
  return ret;
}

/**
 * Function: QCameraPPDgCore::deInit
 *
 * Description: DeInit function of class
 *
 * Arguments:
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
bool QCameraPPDgCore::DeInit()
{
  bool ret = TRUE;
  bIsDeinitStarted = true;

  DGDBG_LOW("E");

  img_q_signal(&callBackQ);
  DGDBG_LOW("before pthread_join");
  pthread_join(threadId, NULL);

  // Reset Thread context to null
  if (m_pThreadData != NULL) {
    funct_pointers.deinit(pLibHdl);
    delete m_pThreadData;
    m_pThreadData = NULL;
  }
  dlclose(pDgipLibFd);

  callback = NULL;

  DGDBG_LOW("X");

  return ret;
}

void QCameraPPDgCore::getDGConfig()
{
  char value[PROPERTY_VALUE_MAX];
  property_get("persist.camera.dg.p2l", value, "0");
  bDgP2LEnabled = (bool) atoi(value);

  mvDGConfig.FrameRateInHz = 30;

  mvDGConfig.camera.focalLength[0] = mvDGConfig.camera.focalLength[1] =
    (mvDGConfig.camera.pixelWidth + mvDGConfig.camera.pixelHeight) / 2;
  mvDGConfig.camera.principalPoint[0] = mvDGConfig.camera.pixelWidth / 2;
  mvDGConfig.camera.principalPoint[1] = mvDGConfig.camera.pixelHeight / 2;
  mvDGConfig.camera.distortionModel = 0;

  if (bDgP2LEnabled) {
    DGDBG_INFO("P2L enabled.");
    mvDGConfig.rotationin[0][0] = 0;
    mvDGConfig.rotationin[0][1] = 0;
    mvDGConfig.rotationin[0][2] = 1;
    mvDGConfig.rotationin[1][0] = -1;
    mvDGConfig.rotationin[1][1] = 0;
    mvDGConfig.rotationin[1][2] = 0;
    mvDGConfig.rotationin[2][0] = 0;
    mvDGConfig.rotationin[2][1] = -1;
    mvDGConfig.rotationin[2][2] = 0;

    mvDGConfig.rotationout[0][0] = 0;
    mvDGConfig.rotationout[0][1] = 0;
    mvDGConfig.rotationout[0][2] = 1;
    mvDGConfig.rotationout[1][0] = 0;
    mvDGConfig.rotationout[1][1] = -1;
    mvDGConfig.rotationout[1][2] = 0;
    mvDGConfig.rotationout[2][0] = 1;
    mvDGConfig.rotationout[2][1] = 0;
    mvDGConfig.rotationout[2][2] = 0;
  }else {
    DGDBG_INFO("P2L disabled.");
    mvDGConfig.rotationin[0][0] = 1.0;
    mvDGConfig.rotationin[0][1] = 0.0;
    mvDGConfig.rotationin[0][2] = 0.0;
    mvDGConfig.rotationin[1][0] = 0.0;
    mvDGConfig.rotationin[1][1] = 1.0;
    mvDGConfig.rotationin[1][2] = 0.0;
    mvDGConfig.rotationin[2][0] = 0.0;
    mvDGConfig.rotationin[2][1] = 0.0;
    mvDGConfig.rotationin[2][2] = 1.0;

    mvDGConfig.rotationout[0][0] = 1.0;
    mvDGConfig.rotationout[0][1] = 0.0;
    mvDGConfig.rotationout[0][2] = 0.0;
    mvDGConfig.rotationout[1][0] = 0.0;
    mvDGConfig.rotationout[1][1] = 1.0;
    mvDGConfig.rotationout[1][2] = 0.0;
    mvDGConfig.rotationout[2][0] = 0.0;
    mvDGConfig.rotationout[2][1] = 0.0;
    mvDGConfig.rotationout[2][2] = 1.0;
  }

  mvDGConfig.zoomFactor = 1.0;
  mvDGConfig.cameraDownTilt = 0.0; //3.14*15/180
  mvDGConfig.useGyro = false;

  //Dump camera config
  DGDBG_INFO("DUMP: camera config");
  DGDBG_INFO("ipW:%d ipH:%d ipStride:%d ipOffset:%d",
      mvDGConfig.camera.pixelWidth,
      mvDGConfig.camera.pixelHeight,
      mvDGConfig.camera.memoryStride,
      mvDGConfig.camera.uvOffset);
  DGDBG_INFO("principalPoint:[%f %f] focalLength:[%f %f]",
      mvDGConfig.camera.principalPoint[0],
      mvDGConfig.camera.principalPoint[1],
      mvDGConfig.camera.focalLength[0],
      mvDGConfig.camera.focalLength[1]);
  DGDBG_INFO("distortion:[%f %f %f %f %f %f %f %f] dist model:%d",
      mvDGConfig.camera.distortion[0],
      mvDGConfig.camera.distortion[1],
      mvDGConfig.camera.distortion[2],
      mvDGConfig.camera.distortion[3],
      mvDGConfig.camera.distortion[4],
      mvDGConfig.camera.distortion[5],
      mvDGConfig.camera.distortion[6],
      mvDGConfig.camera.distortion[7],
      mvDGConfig.camera.distortionModel);

  DGDBG_INFO("opW:%d opH:%d opStride:%d opOffset:%d framerate:%d",
      mvDGConfig.pxlWidth,
      mvDGConfig.pxlHeight,
      mvDGConfig.rowMemoryStride,
      mvDGConfig.uvOffset,
      mvDGConfig.FrameRateInHz);
  DGDBG_INFO("rotation matrix: {%f %f %f} {%f %f %f} {%f %f %f}",
      mvDGConfig.rotationin[0][0], mvDGConfig.rotationin[0][1],mvDGConfig.rotationin[0][2],
      mvDGConfig.rotationin[1][0],mvDGConfig.rotationin[1][1],mvDGConfig.rotationin[1][2],
      mvDGConfig.rotationin[2][0],mvDGConfig.rotationin[2][1],mvDGConfig.rotationin[2][2]);
  DGDBG_INFO("rotationout matrix: {%f %f %f} {%f %f %f} {%f %f %f}",
      mvDGConfig.rotationout[0][0], mvDGConfig.rotationout[0][1], mvDGConfig.rotationout[0][2],
      mvDGConfig.rotationout[1][0], mvDGConfig.rotationout[1][1], mvDGConfig.rotationout[1][2],
      mvDGConfig.rotationout[2][0], mvDGConfig.rotationout[2][1], mvDGConfig.rotationout[2][2]);
  DGDBG_INFO("zoomFactor:%f cameraDownTilt:%f useGyro:%d",
      mvDGConfig.zoomFactor,mvDGConfig.cameraDownTilt,mvDGConfig.useGyro);

}

bool QCameraPPDgCore::startSession()
{
  int32_t status;
  bool ret = true;

  DGDBG_INFO("E");

  if (bIsInputSizeSet && bIsOutputSizeSet) {

    getDGConfig();

    status = funct_pointers.start_session(pLibHdl, &mvDGConfig);
    if (status) {
      DGDBG_ERROR("Failed on start session with status: %d, pLibHdl: %p",
                  status, pLibHdl);
      ret = false;
    }
  } else {
    DGDBG_ERROR("Input/output size is not set!");
    ret = false;
  }
  DGDBG_INFO("X");
  return ret;
}

bool QCameraPPDgCore::stopSession()
{
  int32_t status;
  int32_t frameid;
  status = funct_pointers.stop_session(pLibHdl, &frameid);
  if (status) {
    DGDBG_ERROR("Failed on stop session with status: %d", status);
    return false;
  }
  return true;
}

void QCameraPPDgCore::setInputSizes(img_dim_t *img_dim)
{
  int32_t status;
  DGDBG_INFO("in width: %d, heght: %d, stride: %d, scanline: %d",
             img_dim->width, img_dim->height,
             img_dim->stride, img_dim->scanline);
  mvDGConfig.camera.pixelWidth    = img_dim->width;
  mvDGConfig.camera.pixelHeight   = img_dim->height;
  mvDGConfig.camera.memoryStride  = img_dim->stride;
  mvDGConfig.camera.uvOffset      = img_dim->scanline - img_dim->height;
  bIsInputSizeSet = true;
  if (bIsInputSizeSet && bIsOutputSizeSet) {
    if (false == startSession()) {
      status = IMG_ERR_GENERAL;
    }
  }
}

void QCameraPPDgCore::setOutputSizes(img_dim_t *img_dim)
{
  int32_t status;
  DGDBG_INFO("out width: %d, heght: %d, stride: %d, scanline: %d",
             img_dim->width, img_dim->height,
             img_dim->stride, img_dim->scanline);
  mvDGConfig.pxlWidth  = img_dim->width;
  mvDGConfig.pxlHeight = img_dim->height;
  mvDGConfig.rowMemoryStride = img_dim->stride;
  mvDGConfig.uvOffset = img_dim->scanline - img_dim->height;

  bIsOutputSizeSet = true;
  if (bIsInputSizeSet && bIsOutputSizeSet) {
    if (false == startSession()) {
      status = IMG_ERR_GENERAL;
    }
  }
}

int8_t QCameraPPDgCore::processData(img_frame_t *inFrame[], int inFrameCnt,
  img_frame_t *outFrame[], int outFrameCnt, img_meta_t *p_meta)
{
  int8_t  ret = IMG_SUCCESS;
  int32_t status = 0;
  ppdg_context_t *ppdg_context = NULL;
  mvDGTransfMatrixArrayInfo matrixInfo;
  mvDGCameraFrameContentInfo frameInfo;
  mvDGROI dgRoi;

  if (!p_meta || !p_meta->userdata || !inFrame || !outFrame) {
    DGDBG_ERROR("meta is NULL or userdata is NULL!");
    return IMG_ERR_GENERAL;
  }
  ppdg_context = (ppdg_context_t*)malloc(sizeof(ppdg_context_t));
  if (!ppdg_context) {
    DGDBG_ERROR(" X  ppdg_context NULL");
    return IMG_ERR_NO_MEMORY;
  }
  DGDBG_INFO("Enter - frame id - output: %d %d, input: %d %d",
             outFrame[0]->frame_id, outFrameCnt, inFrame[0]->frame_id, inFrameCnt);

  ppdg_context->user_data = p_meta->userdata;
  ppdg_context->outFrame.frameid = outFrame[0]->frame_id;
  ppdg_context->outFrame.pxlNV12 = outFrame[0]->frame[0].plane[0].addr;
  ppdg_context->outFrame.ionFd = (uint32_t *)&outFrame[0]->frame[0].plane[0].fd;

  outFrame[0]->timestamp = inFrame[0]->timestamp;

  frameInfo.frameid = inFrame[0]->frame_id;
  frameInfo.pxlNV12 = inFrame[0]->frame[0].plane[0].addr;
  frameInfo.ionFd   = (uint32_t *)&inFrame[0]->frame[0].plane[0].fd;
  DGDBG_INFO("set frame content: frameid: %d, addr: %p, ionfd: %d",
             frameInfo.frameid, frameInfo.pxlNV12, frameInfo.ionFd);
  funct_pointers.set_frame_content(pLibHdl, &frameInfo);

  matrixInfo.transfArray = (mvPose3DR *)&p_meta->eis_cfg.transform_matrix[0];
  matrixInfo.frameid = p_meta->frame_id;
  // TODO: use num mesh from DGTC
  matrixInfo.gridW = p_meta->eis_cfg.num_mesh_x;
  matrixInfo.gridH = p_meta->eis_cfg.num_mesh_y;
  matrixInfo.transfMatrixType = HOMOGRAPHY_RSC_COMBO;//LENS_DISTORTION_CORR;//ROLLING_SHUTTER_CORR;

  DGDBG_INFO("gridW: %d, gridH: %d, matrix: %p, frameid: %d", p_meta->eis_cfg.num_mesh_x, p_meta->eis_cfg.num_mesh_y,
             matrixInfo.transfArray, matrixInfo.frameid);

  DGDBG_LOW("trasfArray[0]: %f, %f, %f, %f, %f, %f, %f, %f, %f",
    matrixInfo.transfArray[0].matrix[0][0], matrixInfo.transfArray[0].matrix[0][1], matrixInfo.transfArray[0].matrix[0][2],
    matrixInfo.transfArray[0].matrix[1][0], matrixInfo.transfArray[0].matrix[1][1], matrixInfo.transfArray[0].matrix[1][2],
    matrixInfo.transfArray[0].matrix[2][0], matrixInfo.transfArray[0].matrix[2][1], matrixInfo.transfArray[0].matrix[2][2]);

  funct_pointers.set_frame_transf_matrix(pLibHdl, &matrixInfo);

  DGDBG_LOW("in buff plane cnt: %d, out buff plane cnt: %d",
             inFrame[0]->frame[0].plane_cnt,
             outFrame[0]->frame[0].plane_cnt);

  for (int cnt = 0 ; cnt < outFrame[0]->frame[0].plane_cnt ; cnt++) {
    DGDBG_LOW("in buff plane[%d] addr: %p, lenght: %zu, WxH: %dx%d, stride: %d", cnt,
               inFrame[0]->frame[0].plane[cnt].addr,
               inFrame[0]->frame[0].plane[cnt].length,
               inFrame[0]->frame[0].plane[cnt].width,
               inFrame[0]->frame[0].plane[cnt].height,
               inFrame[0]->frame[0].plane[cnt].stride);
    DGDBG_LOW("out buff plane[%d] addr: %p, lenght: %zu, WxH: %dx%d", cnt,
               outFrame[0]->frame[0].plane[cnt].addr,
               outFrame[0]->frame[0].plane[cnt].length,
               outFrame[0]->frame[0].plane[cnt].width,
               outFrame[0]->frame[0].plane[cnt].height);
  }

  DGDBG_INFO("get out image: frameid: %d, addr: %p, ionfd: %d",
             ppdg_context->outFrame.frameid,
             ppdg_context->outFrame.pxlNV12,
             ppdg_context->outFrame.ionFd);
  status = funct_pointers.get_output_image(
        pLibHdl,
        ppdg_context->outFrame.frameid,
        &ppdg_context->outFrame, &dgRoi);
  if (status) {
    DGDBG_ERROR("Failed to get output image: status: %d", status);
  }

  DGDBG_INFO(": done");

  status = img_q_enqueue(&callBackQ, ppdg_context);
  if (IMG_ERROR(status)) {
    ret = IMG_ERR_GENERAL;
  } else {
    img_q_signal(&callBackQ);
  }

  DGDBG_LOW(": X ret");
  return ret;
}

int QCameraPPDgCore::fillFuncTable(void)
{
  DGDBG_INFO("E");
  dlerror();
  pDgipLibFd = dlopen("/usr/lib/libmvDGIP.so", RTLD_LAZY);

  if (!pDgipLibFd) {
    DGDBG_ERROR("Failed to open libDGIP");
    return 0;
  }

  funct_pointers.init_get_handle =
    (mvDGIP*(*)())dlsym(pDgipLibFd, "mvDGIP_Initialize");
  if(funct_pointers.init_get_handle == NULL) {
    DGDBG_ERROR("init_get_handle fail %s", dlerror());
    return 0;
  }

  funct_pointers.deinit =
    (void (*)(mvDGIP*))dlsym(pDgipLibFd, "mvDGIP_Deinitialize");
  if(funct_pointers.deinit == NULL) {
    DGDBG_ERROR("deinit fail %s", dlerror());
    return 0;
  }

  funct_pointers.start_session =
    (int32_t (*)(mvDGIP*,mvDGConfiguration*))dlsym(pDgipLibFd, "mvDGIP_StartSession");
  if(funct_pointers.start_session == NULL) {
    DGDBG_ERROR("start_session fail %s", dlerror());
    return 0;
  }

  funct_pointers.stop_session =
    (int32_t (*)(mvDGIP*, int32_t*))dlsym(pDgipLibFd, "mvDGIP_StopSession");
  if(funct_pointers.stop_session == NULL) {
    DGDBG_ERROR("stop_session fail %s", dlerror());
    return 0;
  }

  funct_pointers.set_frame_content =
    (void (*)(mvDGIP* pObj, mvDGCameraFrameContentInfo* pImgDataInfo))
      dlsym(pDgipLibFd, "mvDGIP_SetFrameContent");
  if(funct_pointers.set_frame_content == NULL) {
    DGDBG_ERROR("set_frame_content fail %s", dlerror());
    return 0;
  }

  funct_pointers.set_frame_transf_matrix =
    (void (*)(mvDGIP* pObj, mvDGTransfMatrixArrayInfo* pTransfArray))
      dlsym(pDgipLibFd, "mvDGIP_SetFrameTransfMatrix");
  if(funct_pointers.set_frame_transf_matrix == NULL) {
    DGDBG_ERROR("set_frame_transf_matrix fail %s", dlerror());
    return 0;
  }

  funct_pointers.get_output_image =
      (int32_t (*)(mvDGIP* pObj, int32_t frameid,
                   mvDGCameraFrameContentInfo* pOutImgInfo,
                   mvDGROI *pROI))dlsym(pDgipLibFd, "mvDGIP_GetOutputImage");
  if(funct_pointers.get_output_image == NULL) {
    DGDBG_ERROR("get_output_image fail %s", dlerror());
    return 0;
  }
  funct_pointers.get_version =
      (const char * (*)(void))dlsym(pDgipLibFd, "mvDGIP_Version");
  if(funct_pointers.get_version == NULL) {
    DGDBG_ERROR("get_version fail %s", dlerror());
    return 0;
  }

  DGDBG_INFO("X");

  return 1;
}

