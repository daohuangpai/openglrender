#include "AudioPlayerAl.h"
#include  <thread>

AudioDrives AudioPlayerAl::m_kAudioDrives;

inline bool GetWaveALBufferFormat(int bytesPerSample,int channels, ALenum *pulFormat)
{
	bool result = true;
	if (channels == 1) {
		if (bytesPerSample == 2) {
			*pulFormat = alGetEnumValue("AL_FORMAT_MONO16");
		}
		else if (bytesPerSample == 1)
		{
			*pulFormat = alGetEnumValue("AL_FORMAT_MONO8");
		}
		else if (bytesPerSample == 4) {
			*pulFormat = alGetEnumValue("AL_FORMAT_MONO_FLOAT32");
		}
		else {
			result = false;
		}
	}
	else if (channels == 2) {

		if (bytesPerSample == 2) {
			*pulFormat = alGetEnumValue("AL_FORMAT_STEREO16");
		}
		else if (bytesPerSample == 1)
		{
			*pulFormat = alGetEnumValue("AL_FORMAT_STEREO8");
		}
		else if (bytesPerSample == 4) {
			*pulFormat = alGetEnumValue("AL_FORMAT_STEREO_FLOAT32");
		}
		else {
			result = false;
		}
	}
	else {
		result = false;
	}
	return result;
}


bool AudioPlayerAl::Open(int bytesPerSample, int samplesPerSec, int channels)
{
	static bool Uninitdrive = true;
	if (Uninitdrive) {
		if (!m_kAudioDrives.ALFWInitOpenAL(NULL)) {
			return false;
		}
		Uninitdrive = false;
	}

	Close();

	if (!alcGetCurrentContext()) {
		return false;
	}

	if (!GetWaveALBufferFormat(bytesPerSample, channels, &m_BufferFormat)) {
		return false;
	}

	alGetError(); // clear error code  
	alGenSources(1, &m_uiSource);
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		//DisplayALError("alGenBuffers :", error);
		return false;
	}
	alGenBuffers(MAX_QUEUE_NUM, m_uiBuffer);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return false;
	}

	BufferPos = 0;
	//alSourcei(m_uiSource, AL_BUFFER, m_uiBuffer);
	alSourcef(m_uiSource, AL_PITCH, 1.0f);
	alSourcef(m_uiSource, AL_GAIN, 1.0f);
	alSourcei(m_uiSource, AL_LOOPING, AL_FALSE);
	alSpeedOfSound(1.0);

	m_ulFrequency = samplesPerSec;
	int wBitsPerSample = bytesPerSample << 3;
	int nBlockAlign = (wBitsPerSample / 8) * channels;
	//int nAvgBytesPerSec = samplesPerSec * nBlockAlign;
	m_FrameSize = nBlockAlign;
	m_samplesPerSec = samplesPerSec;
	m_IsOpen = true;
	return true;
}


void AudioPlayerAl::Close()
{
	m_IsOpen = false;
	alDeleteSources(1, &m_uiSource);
	alDeleteBuffers(MAX_QUEUE_NUM, m_uiBuffer);
}


bool AudioPlayerAl::WriteAudio(uint8_t* write_data, int64_t write_size)
{
	ALint iState = 0;
AAA:
	if (BufferPos < MAX_QUEUE_NUM)
	{
		alBufferData(m_uiBuffer[BufferPos], m_BufferFormat, write_data, write_size, m_ulFrequency);
		alSourceQueueBuffers(m_uiSource, 1, &m_uiBuffer[BufferPos]);
		BufferPos++;
	}
	else {
		while (true)
		{
			ALint buffer;
			alGetSourcei(m_uiSource, AL_BUFFERS_QUEUED, &buffer);
			if (buffer == 0) {
				BufferPos = 0;
				goto AAA;
			}

			ALint processed = 0;
			alGetSourcei(m_uiSource, AL_BUFFERS_PROCESSED, &processed);
			if (processed > 0)
			{
				ALuint bufferID = 0;
				alSourceUnqueueBuffers(m_uiSource, 1, &bufferID);
				alBufferData(bufferID, m_BufferFormat, write_data, write_size, m_ulFrequency);
				alSourceQueueBuffers(m_uiSource, 1, &bufferID);
				break;
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
			}
		}
		
	}
	ALenum error;
	error = alGetError();
	alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iState);
	if(iState!= AL_PLAYING)
	  alSourcePlay(m_uiSource);

	int frame = write_size / m_FrameSize;
	const double frame_clock = (double)frame / m_samplesPerSec;
	return true;
}


void AudioPlayerAl::SetVolume(double volume)
{
	alSourcef(m_uiSource, AL_GAIN, volume);
}

double AudioPlayerAl::GetVolume() const
{
	ALfloat gain;
	alGetSourcef(m_uiSource, AL_GAIN,&gain);
	return gain;
}


void AudioPlayerAl::WaveOutPause()
{
	ALint iState = 0;
	alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iState);
	if (iState != AL_PAUSED)
		alSourcePause(m_uiSource);
}


void AudioPlayerAl::WaveOutRestart()
{
	ALint iState = 0;
	alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iState);
	if (iState == AL_PLAYING) {
		alSourceStop(m_uiSource);
	}

	alSourceRewind(m_uiSource);
	alSourcePlay(m_uiSource);
}





void AudioDrives::Shutdown()
{
	ALCcontext *pContext;
	ALCdevice *pDevice;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	
	if (pContext) {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(pContext);
	}
		
	if (pDevice)
		alcCloseDevice(pDevice);
}

ALCdevice* AudioDrives::ALFWInitOpenAL(char *szDriveName)
{
	Shutdown();//œ»πÿ±’;

	ALCdevice		*pDevice = NULL;
	ALCcontext		*pContext;
	ALenum error;
	error = alGetError();
	pDevice = alcOpenDevice((ALCchar*)szDriveName);
	//AL_NO_ERROR
	error = alGetError();
	pContext = alcCreateContext(pDevice, NULL);
	error = alGetError();
	alcMakeContextCurrent(pContext);
	error = alGetError();
	return pDevice;
}
