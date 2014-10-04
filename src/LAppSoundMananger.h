#pragma once


//音频文件格式
struct RIFF_HEADER//文件类型
{
	char szRiffID[4];
	DWORD dwRiffSize;
	char szRiffFormat[4];
};

struct WAVE_FORMAT
{
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSamplesPerSec;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
};

struct FMT_BLOCK//格式
{
	char szFmtID[4]; // 'f','m','t',' '
	DWORD dwFmtSize;//////////////一般情况下为16，如有附加信息为18
	WAVE_FORMAT wavFormat;
};

struct FACT_BLOCK//可先项 一般可不用
{
	char szFactID[4]; // 'f','a','c','t'
	DWORD dwFactSize;
};

struct DATA_BLOCK//数据头
{
	char szDataID[4]; // 'd','a','t','a'
	DWORD dwDataSize;//数据长度
};

bool PlayModelSound(wchar_t path[],char p[],int index);
void  StopPlaySound();