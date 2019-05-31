#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "ql_omx_video.h"

#define NUMBER_OF_ARBITRARYBYTES_READ 4 * 1024  ///used in test main

///wait time while queue indata in encode
#define ENCODE_QUEUEIN_WAITTIME 100
///wait time while queue outdata in encode
#define ENCODE_QUEUEOUT_WAITTIME 100  
///wait time while queue indata in decode
#define DECODE_QUEUEIN_WAITTIME 100   
///wait time while queue outdata in decode
#define DECODE_QUEUEOUT_WAITTIME 100 

#define FRAME_MIN_LEN 512       // 此值用于找到第一帧头后，继续寻找第二个帧头，如果解码失败可尝试缩小此值
#define FRAME_MAX_LEN 300*1024  // 一般H264帧大小不超过200k,如果解码失败可尝试增大此值

#define ENCODE_USE
#define DECODE_USE

/*** SPS or pps **/
bool isSpsPpsFrameHeadType(OMX_U8 head) {
    return head == (OMX_U8) 0x67 || head == (OMX_U8) 0x68;
}


/**
* judge I/P NAL header:
* 00 00 00 01 65    (I Frame)
* 00 00 00 01 61 / 41   (P Frame)
*
* @param data
* @param offset
* @return ture if it is I/P Nal header,or fail if not.
*/
bool isSecHead(OMX_U8* data, int offset) {
    bool result = false;
	
	if (data[offset] == 0x00 && data[offset + 1] == 0x00&& data[offset + 2] == 0x00 && data[offset + 3] == 0x01 && !isSpsPpsFrameHeadType(data[offset + 4])) {
    	result = true;
    }
    // 00 00 01 x
    if (data[offset] == 0x00 && data[offset + 1] == 0x00&& data[offset + 2] == 0x01 && !isSpsPpsFrameHeadType(data[offset + 3])) {
        result = true;
    }
    return result;
}

bool isHead(OMX_U8* data, int offset) {
    bool result = false;
	
	if (data[offset] == 0x00 && data[offset + 1] == 0x00&& data[offset + 2] == 0x00 && data[offset + 3] == 0x01) {
    	result = true;
    }
    // 00 00 01 x
    if (data[offset] == 0x00 && data[offset + 1] == 0x00&& data[offset + 2] == 0x01 ) {
        result = true;
    }
    return result;
}


/**
* looking for the location of start code in the buffer
*
* @param data   the source NAL data
* @param offset 
* @param max    the max value 
* @return the location of start code ,and -1 means not find start code
*/
int findHead(OMX_U8* data, int offset, int max) {
    int i;
    for (i = offset; i <= max; i++) {
        // find Nal's header
        if (isHead(data, i))
            break;
    }

    if (i == max) {
        i = -1;
    }
    return i;
}

int findSecHead(OMX_U8* data, int offset, int max) {
    int i;
	bool isFrame = false;
    for (i = offset; i <= max; i++) {
        // find Nal's header
		if (isFrame && isHead(data, i))
		{
			break;
		}
			
        if (isSecHead(data, i))  //not sps|pps
		{
			isFrame = true;
			i += 3;
		}
    }

    if (i == max) {
        i = -1;
    }
    return i;
}


int _checksps_pps(OMX_U8* in, OMX_S32 insize)
{
	printf("decoder input_data insize = %d\n", insize);
	if (insize > 10)
	{
		printf("decoder input_data 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", *(in), *(in+1), *(in+2), *(in+3), *(in+4), *(in+5));
		if (*in == 0x0 && *(in+1) == 0x0 && *(in+2) == 0x0 && *(in+3) == 0x1 && (*(in+4)&0xF)== 0x7)
		{
			return 1;
		}
	}
	return 0;
}

//qcamera::QCameraEncoder *qEncoder = NULL;
//qcamera::QCameraDecoder *qDecoder = NULL;
//int decode_start_flag = 0;
#define DECODER_TEST
#define ENCODER_TEST
int main(int argc, char *argv[])
{
	OMX_U32 width, height, stride, sliceStride;
	int length;
	int nBytesRead = 0;
	int frame_count = 0;

	if (argc < 3)
	{
		printf("help: \n");
		printf("mm-video-test ENCODE|DECODE FILENAME\n");
		return -1;
	}
	
	printf("argv[1] %s\n", argv[1]);
	if (strcmp(argv[1], "ENCODE") != 0 && strcmp(argv[1], "DECODE") != 0)
	{
		printf("help: \n");
		printf("mm-video-test ENCODE|DECODE FILENAME\n");
		return -1;
	}

#ifdef ENCODER_TEST
	if (strcmp(argv[1], "ENCODE") == 0)
	{
		if (argc < 5)
		{
			printf("please input width and height\n");
			return -1;
		}
		
		width = atoi(argv[3]);
		height = atoi(argv[4]);

		printf("encode width = %d, height = %d\n", width, height);
		void *pEnc = NULL;
		OMX_U8 *pEncOutputBuffer = NULL;
		OMX_U8 *pReadBuffer = (OMX_U8 *) malloc(width * height * 3);
	
		struct EncoderInitParam eInitParam;
		eInitParam.width = width;
		eInitParam.height = height;
		eInitParam.bitrate = 1024*1024;
		eInitParam.framerate = 30;
		eInitParam.ratetype = 0;
		eInitParam.intraPeriod = 30;  // I帧间隔
		eInitParam.minQP = 15;  // QP最小值
		eInitParam.maxQP = 30;  // QP最大值
		eInitParam.codecProfileType = CODECPROFILE_AVC_Baseline;
		eInitParam.codecType = CODEC_AVC;
	
		pEnc = (void *) QL_Encoder_Init(eInitParam);
	
		int m_srcYuv_fd = open( argv[2], O_RDONLY | O_LARGEFILE);
		if (m_srcYuv_fd < 0)
		{
			printf("open yuv file %s\n", argv[2]);
			return -1;
		}
		FILE *dest = fopen("/data/enc.h264", "w+");
		if (dest == NULL)
		{
			perror("open /data/enc.h264 failed\n");
			return -1;
		}

		for (int i=0;;i++)
		{
			memset(pReadBuffer, 0, width*height*3);
			nBytesRead = read(m_srcYuv_fd, pReadBuffer, width * height * 3 / 2);
			if (nBytesRead <= 0)
			{
				printf("read file end\n");
				break;
			}
			printf("start read nBytesRead = %d count = %d\n", nBytesRead, i);
			pEncOutputBuffer = GL_Encoder_Process(pEnc, pReadBuffer, nBytesRead, i * 1000000 / 30, &length);
			if (pEncOutputBuffer != NULL)
			{
				fwrite(pEncOutputBuffer, 1, length, dest);
			}
		}

//get left data
		/*usleep(100*1000);
		while(true)
		{
			length = 0;
			pEnc->dequeueOutputBuffer(&pEncOutputBuffer, (OMX_S32 *) &length);
			if (length == 0)
			{
				break;
			}
			if (pEncOutputBuffer != NULL)
			{
				fwrite(pEncOutputBuffer, 1, length, dest);
			}
		}*/

    // release
		if(dest)
		{
			fclose(dest);
			dest = NULL;
		}	
	
		if(m_srcYuv_fd)
		{
			close(m_srcYuv_fd);
			m_srcYuv_fd = 0;
		}
	
		if(pReadBuffer)
		{
			free(pReadBuffer);
			pReadBuffer = NULL;
		}

		GL_Encoder_Release(pEnc);
	}
#endif

	
#ifdef DECODER_TEST
	if (strcmp(argv[1], "DECODE") == 0)
	{
		int m_srcFile_fd = open( argv[2], O_RDONLY | O_LARGEFILE);
		if (m_srcFile_fd < 0)
		{
			printf("open file %s failed \n", argv[2]);
			return -1;
		}

		FILE *dest_out = fopen("/data/dec.yuv", "w+");
		OMX_U8 *pDecOutputBuffer = NULL;
		
		void *pDec = NULL; 
		pDec = (void *)GL_Decoder_Init(CODEC_AVC);	
		
		OMX_U8 *pReadDecodeBuffer = (OMX_U8 *) malloc(10*1024); // Read the data from Video file 
		OMX_U8 *pProcessData = (OMX_U8*)malloc(FRAME_MAX_LEN);       // the data prepare for search the head of NAL
		OMX_U8 *pOneframeData = (OMX_U8*)malloc(FRAME_MAX_LEN);      // for saving nal of one frame
		OMX_U8 *pTempframe = (OMX_U8*)malloc(FRAME_MAX_LEN);         // intermediate data point
		int nFrameLen = 0; // current frame length
		int nCount = 0;   // frame count
		
		
		while (1) {
			
			nBytesRead = read(m_srcFile_fd, pReadDecodeBuffer, 10 * 1024);
    	    if (nBytesRead <= 0)
    	    {
    	        break;
    	    }

    	    if (nFrameLen + nBytesRead < FRAME_MAX_LEN) {

    	        memcpy(pProcessData + nFrameLen, pReadDecodeBuffer, nBytesRead);  //copy the data of pReadDecodeBuffer to pProcessData
				
    	        nFrameLen += nBytesRead;   //modify nFrameLen
    	        int headFirstIndex = findHead(pProcessData, 0, nFrameLen);  //looking for the first header of Nal 
    	        while (headFirstIndex >= 0) {
					int headSecondIndex;
					if (isSpsPpsFrameHeadType(*(pProcessData+headFirstIndex+3)) || isSpsPpsFrameHeadType(*(pProcessData+headFirstIndex+4)))  //如果当前是sps|pps,寻找的下一个不能是sps|pps
					{
						headSecondIndex = findSecHead(pProcessData, headFirstIndex + 4, nFrameLen);  //looking for the second header of Nal 
					} else
					{
						headSecondIndex = findHead(pProcessData, headFirstIndex + 4, nFrameLen);  //looking for the second header of Nal 
					}

    	            //if the second header of NAL exist，the one frame data is the data between first header and second header
    	            if (headSecondIndex > 0 && isHead(pProcessData, headSecondIndex)) {

    	                //video Decoder : Start
    	                memset(pOneframeData, 0, FRAME_MAX_LEN);  // reset the data of Oneframe
    	                memcpy(pOneframeData, pProcessData+headFirstIndex, headSecondIndex - headFirstIndex);  // get the data of one frame

    	                pDecOutputBuffer = GL_Decoder_Process(pDec, pOneframeData, headSecondIndex - headFirstIndex, nCount * 1000000 / 30, &width, &height, &stride, &sliceStride);  // the real decoder
									  
    	                if (pDecOutputBuffer != NULL){
    	                    frame_count++;
    	                    fwrite(pDecOutputBuffer, 1, stride * sliceStride * 3 / 2, dest_out);
    	                }
    	                printf("frame_count = %d, nLen = %d, stride = %d, sliceStride = %d\n", frame_count, headSecondIndex - headFirstIndex, stride, sliceStride); // print diff time
    	                //video Decoder : End
						
    	                nFrameLen = nFrameLen-headSecondIndex; //modify nFrameLen
    	                memcpy(pTempframe, pProcessData+headSecondIndex, nFrameLen); //copy the rest data of pProcessData to pTempframe
    	                memset(pProcessData, 0, FRAME_MAX_LEN);
    	                memcpy(pProcessData, pTempframe, nFrameLen); //copy the data of pTempframe to pProcessData
						
    	                headFirstIndex = findHead(pProcessData, 0, nFrameLen); //going to looking for the header of NAL
					
    	            } else {  // if can not find the sencond header of NAL         
    	                headFirstIndex = -1;
    	            }
    	        }
    	    } else {  // if the length is too long 
    	        nFrameLen = 0;
    	    }
		}

//get left yuv data
		/*usleep(1000*1000);
		while(true)
		{
			length = 0;
			pDec->dequeueOutputBuffer(&pDecOutputBuffer, (OMX_S32 *) &length);
			if (length == 0)
			{
				break;
			}
			if (pDecOutputBuffer != NULL)
			{
				fwrite(pDecOutputBuffer, 1, length, dest_out);
			}
		}*/


		
		//release
		
		if(m_srcFile_fd)
		{
			close(m_srcFile_fd);
			m_srcFile_fd = 0; 
		}
		
		if(dest_out)
		{
			fclose(dest_out);
			dest_out = NULL;
		}
		
		if(pReadDecodeBuffer)
		{
			free(pReadDecodeBuffer);
			pReadDecodeBuffer = NULL;
		}
		
		if(pProcessData)
		{
			free(pProcessData);
			pProcessData = NULL;
		}
		
		if(pOneframeData)
		{
			free(pOneframeData);
			pOneframeData = NULL;
		}
		
		if(pTempframe)
		{
			free(pTempframe);
			pTempframe = NULL;
		}
		
		GL_Decoder_Release(pDec);
	}
#endif

}
