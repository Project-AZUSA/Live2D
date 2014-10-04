#include "main.h"
#include "LAppSoundMananger.h"

LPDIRECTSOUNDBUFFER g_lpdbsBuffer = NULL;
DSBUFFERDESC g_dsbd;
WAVEFORMATEX g_wfmx;
char* g_sndBuffer = NULL;
LPDIRECTSOUND g_lpds = NULL;
BOOL Loadwav(_TCHAR *FileName,UINT Flag);
#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS)
#endif

extern bool	isPlayingSound;
extern int		AzusaPid;
extern int		SoundIndex;

extern HANDLE								g_MsgThread;
extern HWND								g_hWindow;

extern  LAppLive2DManager*		s_live2DMgr;


//播放音效
bool LoadWav(_TCHAR *FileName,UINT Flag)
{
	HMMIO handle ;
	MMCKINFO mmckriff,mmckIn;
	PCMWAVEFORMAT pwfm;
	memset(&mmckriff,0,sizeof(MMCKINFO));
	if((handle= mmioOpen(FileName,NULL,MMIO_READ|MMIO_ALLOCBUF))==NULL)
		return FALSE;
	if(0 !=mmioDescend(handle,&mmckriff,NULL,0))
	{
		mmioClose(handle,0);
		return FALSE;
	}
	if(mmckriff.ckid !=FOURCC_RIFF||mmckriff.fccType !=mmioFOURCC('W','A','V','E'))
	{
		mmioClose(handle,0);
		return FALSE;
	}
	mmckIn.ckid = mmioFOURCC('f','m','t',' ');
	if(0 !=mmioDescend(handle,&mmckIn,&mmckriff,MMIO_FINDCHUNK))
	{
		mmioClose(handle,0);
		return FALSE;
	}
	if(mmioRead(handle,(HPSTR)&pwfm,sizeof(PCMWAVEFORMAT))!=sizeof(PCMWAVEFORMAT))
	{
		mmioClose(handle,0);
		return FALSE;
	} 
	if(pwfm.wf.wFormatTag != WAVE_FORMAT_PCM)
	{
		mmioClose(handle,0);
		return FALSE;
	}
	memcpy(&g_wfmx,&pwfm,sizeof(pwfm));
	g_wfmx.cbSize =0; 
	if(0 != mmioAscend(handle,&mmckIn,0))
	{
		mmioClose(handle,0);
		return FALSE;
	}
	mmckIn.ckid = mmioFOURCC('d','a','t','a');
	if(0 !=mmioDescend(handle,&mmckIn,&mmckriff,MMIO_FINDCHUNK))
	{
		mmioClose(handle,0);
		return FALSE;
	} 
	g_sndBuffer = new char[mmckIn.cksize];
	mmioRead(handle,(HPSTR)g_sndBuffer,mmckIn.cksize); mmioClose(handle,0);
	g_dsbd.dwSize = sizeof(DSBUFFERDESC);
	g_dsbd.dwBufferBytes =mmckIn.cksize;
	g_dsbd.dwFlags = DSBCAPS_CTRLDEFAULT;
	g_dsbd.lpwfxFormat =&g_wfmx;
	if(FAILED(g_lpds ->CreateSoundBuffer(&g_dsbd,&g_lpdbsBuffer,NULL)))
	{
		delete [] g_sndBuffer;
		return FALSE;
	}
	VOID* pDSLockedBuffer =NULL;
	DWORD dwDSLockedBufferSize =0;
	if(g_lpdbsBuffer ->Lock(0,mmckIn.cksize,&pDSLockedBuffer,&dwDSLockedBufferSize,NULL,NULL,0L))
		return FALSE;
	memcpy(pDSLockedBuffer,g_sndBuffer,mmckIn.cksize); 
	if(FAILED(g_lpdbsBuffer ->Unlock(pDSLockedBuffer,dwDSLockedBufferSize,NULL,0)))
	{
		delete [] g_sndBuffer;
		return FALSE;
	} 
	return TRUE;
}


DWORD WINAPI SoundMouth(LPVOID lpParam)
{
	LAppModel* model=	s_live2DMgr->getModel(SoundIndex);
	RIFF_HEADER riff;
	WAVE_FORMAT wform;
	FMT_BLOCK fmt;
	FACT_BLOCK fact;
	DATA_BLOCK data;

	FILE* file=(FILE*)lpParam;
	fread(&riff,sizeof(RIFF_HEADER),1,file);//读RIFF_HEADER 

	if(riff.szRiffFormat[0]!='W'||riff.szRiffFormat[1]!='A'&&riff.szRiffFormat[2]!='V'&&riff.szRiffFormat[3]!='E')
	{
		cout<<"音频格式非法"<<endl;
		return false;
	}

	fread(&fmt,sizeof(FMT_BLOCK),1,file);//读FMT_BLOCK
	if(fmt.dwFmtSize==18)//有额外信息需要读掉，否则后面会出错
	{
		WORD extra;
		fread(&extra,sizeof(WORD),1,file);
		if(extra>0)
		{
			BYTE *ed=new BYTE[extra];
			fread(ed,sizeof(BYTE),extra,file);
			delete[] ed;
		}
	}
	while(true)//循环读入文件块，直到读到音频数据
	{
		fread(&fact.szFactID,sizeof(char),4,file);//读下一块名称
		fread(&fact.dwFactSize,sizeof(DWORD),1,file);//读下一长度
		if(fact.szFactID[0]=='d'&&fact.szFactID[1]=='a'&&fact.szFactID[2]=='t'&&fact.szFactID[3]=='a')
		{

			data.szDataID[0]='d';data.szDataID[1]='a';data.szDataID[2]='t';data.szDataID[3]='a';
			data.dwDataSize=fact.dwFactSize;
			break;
		}
		else
		{
			BYTE * fd=new BYTE[fact.dwFactSize];
			fread(fd,sizeof(BYTE),fact.dwFactSize,file);
			delete[] fd;
		}
	}
	WORD bytes = (WORD)((fmt.wavFormat.wBitsPerSample + 7) / 8);
	DWORD samplenum=data.dwDataSize/bytes;
	BYTE *bd;
	short *sd;
	if(bytes==1)
	{
		bd=new BYTE[samplenum];
		fread(bd,sizeof(BYTE),samplenum,file);//写音频数据
	}
	else
	{
		sd=new short[samplenum];
		fread(sd,sizeof(short),samplenum,file);
	}
	DWORD i;
	float *dm=new float[samplenum];//采样频率
	float df = 1.0f / (1l << (fmt.wavFormat.wBitsPerSample- 1));
	for (i = 0; i < samplenum; i ++ ) 
		dm[i] = ((bytes == 1) ? bd[i] - 128 : sd[i]) * df;
	if (bytes == 1) delete[] bd; else delete[] sd;
	fclose(file);
	WORD count=1;
	//5采样计算平均值以便同步
	int freq=100;//采样频率
	DWORD mouthnum=samplenum/(fmt.wavFormat.dwSamplesPerSec/freq)+1;
	DWORD diff=(fmt.wavFormat.dwSamplesPerSec/freq*fmt.wavFormat.wChannels*bytes);
	float *ma=new float[mouthnum];
	for(i=0;i<mouthnum;i++)
	{
		float avg=0;
		for(DWORD j=i*fmt.wavFormat.dwSamplesPerSec/freq*fmt.wavFormat.wChannels;j<(i+1)*fmt.wavFormat.dwSamplesPerSec/freq*fmt.wavFormat.wChannels&&j<samplenum;j++)
		{
			avg+=fabs(dm[j]);
		}
		ma[i]=avg/(fmt.wavFormat.dwSamplesPerSec/freq);
	}
	delete[] dm;
	g_lpdbsBuffer->Play(0,0,0);
	DWORD pc=0;
	g_lpdbsBuffer->GetCurrentPosition(&pc,0);
	int num=0;
	while(isPlayingSound&&num<5)
	{
		g_lpdbsBuffer->GetCurrentPosition(&pc,0);
		model->mouthY=ma[pc/diff]*5;
		Sleep(10);
		if(pc==0)
		{
			num++;
		}
	}
	delete[] ma;
	isPlayingSound=false;
	model->isSpeaking=false;
	return false;
}


bool PlayModelSound(wchar_t path[],char p[],int index)
{
	try
	{
		if(isPlayingSound==true)
		{
			cout<<"上一个音频未播放完成"<<endl;return false;
		}
		if(index>=s_live2DMgr->getModelNum())
		{
			cout<<"模型序号越界"<<endl;
			return false;
		}
		LAppModel* model=	s_live2DMgr->getModel(index);
		SoundIndex=index;
		FILE* file=fopen(p,"rb");
		if(!file)
		{
			cout<<"文件不存在"<<endl;return false;
		}
		if(DirectSoundCreate(NULL,&g_lpds,NULL) != DS_OK)
		{return false;}
		if(g_lpds ->SetCooperativeLevel(g_hWindow,DSSCL_NORMAL)!=DS_OK)
		{return false;}
		model->isSpeaking=true;
		bool pl=LoadWav(path,DSBCAPS_CTRLDEFAULT);
		if(pl==false)
		{
			isPlayingSound=false;
			return false;
		}
		isPlayingSound=true;
		g_MsgThread = CreateThread(NULL,0,SoundMouth,(LPVOID)file,0,NULL);
		return pl;
	}
	catch(...){
		return false;
	}
}

void StopPlaySound()
{
	if(g_lpdbsBuffer)
	{
		g_lpdbsBuffer->Stop();
	}
}