/*
* 搭载 AZUSA 使用的 Live2D 整合界面
*
* 界面基于 Live2D SDK for DirectX 2.0.06
* 
* LAppDecode.cpp 音频解码
*/

#include "LAppDecode.h"

int EncFmt;

void writeWavHeader(AVCodecContext *pCodecCtx,AVFormatContext *pFormatCtx,FILE *audioFile,int l) {
	std::string data;
    int32_t long_temp;
    int16_t short_temp;
    int16_t BlockAlign;
    int bits=16;
    int32_t fileSize;
    int64_t audioDataSize=l-44;

	pCodecCtx->channels=1+EncFmt;

    switch(pCodecCtx->sample_fmt) {
        case AV_SAMPLE_FMT_S16:
            bits=16;
            break;
        case AV_SAMPLE_FMT_S32:
            bits=32;
            break;
        case AV_SAMPLE_FMT_U8:
            bits=8;
            break;
        default:
            bits=16;
            break;
    }
    fileSize=audioDataSize+36;
    data="RIFF";
    fwrite(data.c_str(),sizeof(char),4,audioFile);
    fwrite(&fileSize,sizeof(int32_t),1,audioFile);

    //"WAVE"
    data="WAVE";
    fwrite(data.c_str(),sizeof(char),4,audioFile);
    data="fmt ";
    fwrite(data.c_str(),sizeof(char),4,audioFile);
    long_temp=16;
    fwrite(&long_temp,sizeof(int32_t),1,audioFile);
    short_temp=0x01;
    fwrite(&short_temp,sizeof(int16_t),1,audioFile);
    short_temp=(pCodecCtx->channels);
    fwrite(&short_temp,sizeof(int16_t),1,audioFile);
    long_temp=(pCodecCtx->sample_rate);
    fwrite(&long_temp,sizeof(int32_t),1,audioFile);
    long_temp=(bits/8)*(pCodecCtx->channels)*(pCodecCtx->sample_rate);
    fwrite(&long_temp,sizeof(int32_t),1,audioFile);
    BlockAlign=(bits/8)*(pCodecCtx->channels);
    fwrite(&BlockAlign,sizeof(int16_t),1,audioFile);
    short_temp=(bits);
    fwrite(&short_temp,sizeof(int16_t),1,audioFile);
    data="data";
    fwrite(data.c_str(),sizeof(char),4,audioFile);
    fwrite(&audioDataSize,sizeof(int32_t),1,audioFile);

    fseek(audioFile,44,SEEK_SET);

}


int myEncode(char pathin[],char pathout[])
{
	int audioStream;
	av_register_all();
	AVFormatContext *pFormatCtx=NULL;
	AVCodecContext  *pCodecCtx=NULL;  
	AVCodec         *pCodec=NULL;
	int d=1;

	// Open video file
	if(avformat_open_input (&pFormatCtx, pathin, NULL, NULL)!=0)
		return -1; // Couldn't open file
    if(av_find_stream_info(pFormatCtx)<0)    
        return -1;
	if(strstr(pFormatCtx->iformat->extensions,"mp3")==NULL&&strstr(pFormatCtx->iformat->extensions,"ape")==NULL)
	{
		if(strstr(pFormatCtx->iformat->extensions,"flac")==NULL)
		{
			return -1;
		}
		else
		{
			EncFmt=1;
		}
	}
	else
	{
		EncFmt=0;
	}

	audioStream=-1;  
    for(int i=0; i < pFormatCtx->nb_streams; i++)  
	{
        //圻葎codec_type==CODEC_TYPE_AUDIO  
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)  
        {  
            audioStream=i;  
            break;  
        } 
	}

	if(audioStream==-1)  
    {  
        printf("Didn't find a audio stream.\n");  
        return -1;  
    }  

	// Get a pointer to the codec context for the audio stream  
	pCodecCtx=pFormatCtx->streams[audioStream]->codec;  

	// Find the decoder for the audio stream  
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)  
    {  
        printf("Codec not found.\n");  
        return -1;  
    }  
  
    // Open codec  
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)  
    {  
        printf("Could not open codec.\n");  
        return -1;  
    } 


	FILE *pFile;  
	pFile=fopen(pathout, "wb");  
	if(!pFile)
	{
		return -1;
	}
    fseek(pFile, 44, SEEK_SET); //圓藻猟周遊議了崔  

    AVPacket *packet=(AVPacket *)malloc(sizeof(AVPacket));  
    av_init_packet(packet);  

    AVFrame *pFrame;  
    pFrame=avcodec_alloc_frame();  

	uint32_t ret,len = 0;
	int got_picture;  
    int index = 0;  
    while(av_read_frame(pFormatCtx, packet)>=0)  
    {  
        if(packet->stream_index==audioStream)  
        {  
			while(1)
			{
				ret = avcodec_decode_audio4( pCodecCtx, pFrame,  
					&got_picture, packet);  
				if ( ret < 0 ) // if error len = -1  
				{  
					printf("Error in decoding audio frame.\n");  
					return -1; 
				}  
				if ( got_picture > 0 )  
				{  
					index=0;
					fwrite(pFrame->data[0], 1, pFrame->linesize[0], pFile);   
					index++;  
				}  
				if(ret!=packet->size)
				{
					packet->data+=ret;
				}
				else
				{
					break;
				}
			}
        }  
        av_free_packet(packet);  
    }  


	fseek(pFile, 0L, SEEK_END);
	int l=ftell(pFile);
	fseek(pFile, 0, SEEK_SET);  
	writeWavHeader(pCodecCtx,pFormatCtx,pFile,l);

	fclose(pFile);

	av_close_input_file(pFormatCtx);

	return 0;
}