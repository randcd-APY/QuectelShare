/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * video decoding with libavcodec API example
 *
 * @example decode_video.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#define INBUF_SIZE 1920*1080
FILE* outputFile;

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

int findStartCode (unsigned char *in_buf, int zeros_in_startcode)  
{  
    int info;  
    int i;  
  
    info = 1;  
    for (i = 0; i < zeros_in_startcode; i++)  
        if(in_buf[i] != 0)  
            info = 0;  
  
    if(in_buf[i] != 1)  
        info = 0;  
    return info;  
}  
  
int getNextNal(FILE* f_in, unsigned char* in_buf)  
{  
    int pos = 0;  
    int startCodeFound = 0;  
    int info2 = 0;  
    int info3 = 0;  
	int nalTypePos = 4;
    while(!feof(f_in) && (in_buf[pos++]=fgetc(f_in))==0);  
  
    while (!startCodeFound)  
    {  
        if (feof (f_in))  
        {  
            //          return -1;  
            return pos-1;  
        }  
        in_buf[pos++] = fgetc (f_in);  
        info3 = findStartCode(&in_buf[pos-4], 3);  
        if(info3 != 1)  
            info2 = findStartCode(&in_buf[pos-3], 2);  
        startCodeFound = (info2 == 1 || info3 == 1);  
	
		if (startCodeFound == 1 && (in_buf[nalTypePos] == 0x67 || in_buf[nalTypePos] == 0x68))
		{
			info2 = 0;
			info3 = 0;
			startCodeFound = 0;
			nalTypePos = pos;
		}
    }  
    fseek (f_in, -4, SEEK_CUR);  
    return pos - 4;  
}  

static int decode_write_frame(AVCodecContext *avctx,
                              AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;
    char buf[1024];
	int i;

    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        printf("Saving %sframe %3d\n", last ? "last " : "", *frame_count);
        fflush(stdout);

        /* the picture is allocated by the decoder, no need to free it */
        /*pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);*/

		printf("frame->linesize[0] = %d frame->linesize[1] = %d frame->linesize[2] = %d\n", frame->linesize[0], frame->linesize[1], frame->linesize[2]);
		for (i=0; i<frame->height; i++)   
    	{   
			fwrite(frame->data[0]+i*frame->linesize[0], 1, frame->width, outputFile);
    	}   

    	for (i=0; i<frame->height/2; i++)   
    	{   
			fwrite(frame->data[1]+i*frame->linesize[1], 1, frame->width/2, outputFile);
    	}   

		for (i=0; i<frame->height/2; i++)   
    	{   
			fwrite(frame->data[2]+i*frame->linesize[2], 1, frame->width/2, outputFile);
    	}  

        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *filename, *outfilename;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
	AVFormatContext *formatContext;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;
	int ret;
	char err_buf[1024];

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    filename    = argv[1];
    outfilename = argv[2];

    av_register_all();

    av_init_packet(&avpkt);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);  //AV_CODEC_ID_MPEG1VIDEO 
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
        c->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames


 	formatContext = avformat_alloc_context();
    ret = avformat_open_input(&formatContext, filename, NULL, NULL);
	if (ret < 0)
	{
		av_strerror(ret, err_buf, 1024);
		printf("******** Decode avformat_open_input() Function result=%d err_buf = %s",ret, err_buf);
		return ret;
	}
 
	if ((ret = avformat_find_stream_info(formatContext, NULL)) < 0) 
	{  
		printf("******** Decode avformat_find_stream_info() Function result=%d ",ret);
		avformat_close_input(&formatContext);  
		return ret;  
	}  
 
	for (int i=0;i<formatContext->streams[0]->codec->extradata_size;i++)
	{
		printf("%x ",formatContext->streams[0]->codec->extradata[i]);
	}

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

	outputFile = fopen(outfilename, "w+");
	if (!outputFile)
	{
		fprintf(stderr, "Could not open outputFile %s\n", outfilename);
		exit(1);
	}

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    frame_count = 0;
    while(!feof(f)) {
        /* avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;
        avpkt.data = inbuf; */

        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
           and this is the only method to use them because you cannot
           know the compressed data size before analysing it.

           BUT some other codecs (msmpeg4, mpeg4) are inherently frame
           based, so you must call them with all the data for one
           frame exactly. You must also initialize 'width' and
           'height' before initializing them. */

        /* NOTE2: some codecs allow the raw parameters (frame size,
           sample rate) to be changed at any frame. We handle this, so
           you should also take care of it */

        /* here, we use a stream based decoder (mpeg1video), so we
           feed decoder and see if it could decode a frame */
		//printf("data size = %d\n", avpkt.size);
		//printf("data 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", avpkt.data[0], avpkt.data[1], avpkt.data[2], avpkt.data[3], avpkt.data[4], avpkt.data[5], avpkt.data[6], avpkt.data[7], avpkt.data[8]);
		avpkt.size = getNextNal(f, inbuf);
		avpkt.data = inbuf;
		printf("avpkt.size = %d\n", avpkt.size);
        while (avpkt.size > 0)
            if (decode_write_frame(c, frame, &frame_count, &avpkt, 0) < 0)
                exit(1);
    }

    /* Some codecs, such as MPEG, transmit the I- and P-frame with a
       latency of one frame. You must do the following to have a
       chance to get the last frame of the video. */
    avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(c, frame, &frame_count, &avpkt, 1);

    fclose(f);
	fclose(outputFile);

    avcodec_free_context(&c);
    av_frame_free(&frame);

    return 0;
}
