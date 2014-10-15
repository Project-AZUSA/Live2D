#pragma once

#include <string>
#include <string.h>

#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif 

#ifdef __cplusplus 
extern "C" {
#endif 
	/*Include ffmpeg header file*/
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h> 

#include <libavutil/imgutils.h>  
#include <libavutil/opt.h>     
#include <libavutil/mathematics.h>   
#include <libavutil/samplefmt.h>

#ifdef __cplusplus 
}
#endif 

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 2<<20 

int myEncode(char pathin[],char pathout[]);
void writeWavHeader(AVCodecContext *pCodecCtx,AVFormatContext *pFormatCtx,FILE *audioFile,int l);