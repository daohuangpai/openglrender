#pragma once

#include "D:\code\openal-soft-master\openal-soft-master\include\AL\al.h"
#include "D:\code\openal-soft-master\openal-soft-master\include\AL\alc.h"
#include <stdint.h>
#include <vector>

class AudioDrives {
public:
	AudioDrives() { /*ALFWInitOpenAL(nullptr);*/ }
	virtual~AudioDrives() { Shutdown(); } 
	void Shutdown();//关闭当前的设备;
	ALCdevice* ALFWInitOpenAL(char *szDriveName);//打开设备;
};

#define MAX_QUEUE_NUM 2

class AudioPlayerAl 
{
public:
	AudioPlayerAl() {}
	virtual ~AudioPlayerAl() { Close(); }

	AudioPlayerAl(const AudioPlayerAl&) = delete;
	AudioPlayerAl& operator=(const AudioPlayerAl&) = delete;



	void Close() ;
	bool Open(int bytesPerSample, int samplesPerSec, int channels) ;

	void SetVolume(double volume) ;
	double GetVolume() const ;

	void WaveOutPause() ;
	void WaveOutRestart() ;

	bool WriteAudio(uint8_t* write_data, int64_t write_size) ;

	bool isOpen() { return m_IsOpen; }

private:

	ALuint		  m_uiSource;
	ALuint		  m_uiBuffer[MAX_QUEUE_NUM];

	//void*		  m_pDecodeBuffer;
	ALenum		  m_BufferFormat;
	ALsizei       m_ulFrequency;

	int m_FrameSize;
	int m_samplesPerSec;
	int BufferPos;

	static AudioDrives m_kAudioDrives;
	bool m_IsOpen = false;
};
