#include "audioPlayer.h"
#include "stringUtility.h"
#include "settingsManager.h"
#include "fileSystem.h"
#include "pointerMap.h"
#include "context.h"
#include "utils.h"

#include <dmusici.h>
#include <dsound.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.cpp"
#include "stb.h"

#define AUDIO_PACKETS 64
#define AUDIO_OUTPUT_BUF_SIZE 8192

extern "C"
{
  extern BOOL g_fDirectSoundDisableBusyWaitWarning;
}

namespace {

	pointerMap<audioContainer*>* mAudioContainerMap;
	LPDIRECTSOUND8 mDirectSoundDevice;
	uint32_t mUniqueSoundId;
	bool mPause;
	CRITICAL_SECTION mPlayerMutex;
}

bool audioPlayer::init()
{
	InitializeCriticalSection(&mPlayerMutex);

	g_fDirectSoundDisableBusyWaitWarning = TRUE;

	mAudioContainerMap = new pointerMap<audioContainer*>(true);
	mUniqueSoundId = 0;

	HRESULT hr = DirectSoundCreate( NULL, &mDirectSoundDevice, NULL );
	return SUCCEEDED(hr);
}

bool audioPlayer::close()
{
	for (uint32_t i = 0; i < mAudioContainerMap->count(); i++)
	{
		audioContainer* sound = mAudioContainerMap->get(i);
		EnterCriticalSection(&mPlayerMutex);
		sound->requestStop = true;
		LeaveCriticalSection(&mPlayerMutex);
	}
	delete(mAudioContainerMap);
	if (mDirectSoundDevice) 
	{
		mDirectSoundDevice->Release();
		mDirectSoundDevice = NULL;
	}
	DeleteCriticalSection(&mPlayerMutex);
	return true;
}

uint32_t audioPlayer::play(const char* soundName, bool repeat)
{
	if (mPause == true || mAudioContainerMap->count() >= 20)
	{
		return 0;
	}
	
	char* soundPackPath = context::getSoundPackPath();
	if (strlen(soundPackPath) == 0)
	{
		free(soundPackPath);
		return 0;
	}

	mUniqueSoundId++;

	audioContainer* sound = new audioContainer();
	sound->id = mUniqueSoundId;
	sound->filePath =  fileSystem::combinePath(soundPackPath, soundName);
	sound->repeat = repeat;
	sound->requestStop = false;
	sound->thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)sound, 0, NULL);
	mAudioContainerMap->add(mUniqueSoundId, sound);

	utils::debugPrint("Sound %i play start with thread handle %i\n", mUniqueSoundId, sound->thread);

	free(soundPackPath);

	return mUniqueSoundId;
}

bool audioPlayer::stop(uint32_t key)
{
	audioContainer* sound = mAudioContainerMap->get(key);
	if (sound == NULL)
	{
		return false;
	}
	
	EnterCriticalSection(&mPlayerMutex);
	sound->requestStop = true;
	LeaveCriticalSection(&mPlayerMutex);
	return true;
}

void audioPlayer::pause(bool value)
{
	mPause = value;
}

uint64_t WINAPI audioPlayer::process(void* param)
{
	audioContainer* container = (audioContainer*)param;
	bool repeat = container->repeat;

	int error;
	stb_vorbis *vorbis = stb_vorbis_open_filename(container->filePath, &error, NULL);
	if (vorbis == NULL)
	{
		return 0;
	}

	stb_vorbis_info vorbisInfo = stb_vorbis_get_info(vorbis);

	WAVEFORMATEX waveFormat;
	memset(&waveFormat, 0, sizeof(waveFormat));
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = vorbisInfo.channels;
	waveFormat.nSamplesPerSec = vorbisInfo.sample_rate;
	waveFormat.wBitsPerSample = sizeof(short) * 8;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample >> 3) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
	
	DSSTREAMDESC streamDesc;
	memset(&streamDesc, 0, sizeof(streamDesc));
	streamDesc.dwMaxAttachedPackets = AUDIO_PACKETS;
	streamDesc.lpwfxFormat = &waveFormat;

	LPDIRECTSOUNDSTREAM	directSoundStream;
	mDirectSoundDevice->CreateSoundStream(&streamDesc, &directSoundStream, NULL); 

	float percent = repeat ? (settingsManager::getMusicVolume() / 100.0f) : (settingsManager::getSoundVolume() / 100.0f);
	int volume = (int)(DSBVOLUME_HW_MIN * (1.0f - min(max(percent, 0.0f), 1.0f)));
	directSoundStream->SetVolume(volume);
	directSoundStream->SetHeadroom(0);

	DSMIXBINVOLUMEPAIR mixBinVolumePair[6];
    mixBinVolumePair[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
    mixBinVolumePair[0].lVolume = DSBVOLUME_MAX;
    mixBinVolumePair[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
    mixBinVolumePair[1].lVolume = DSBVOLUME_MAX;
    mixBinVolumePair[2].dwMixBin = DSMIXBIN_FRONT_CENTER;
    mixBinVolumePair[2].lVolume = DSBVOLUME_MIN;
    mixBinVolumePair[3].dwMixBin = DSMIXBIN_LOW_FREQUENCY;
    mixBinVolumePair[3].lVolume = DSBVOLUME_MIN;
    mixBinVolumePair[4].dwMixBin = DSMIXBIN_BACK_LEFT;
    mixBinVolumePair[4].lVolume = DSBVOLUME_MIN;
    mixBinVolumePair[5].dwMixBin = DSMIXBIN_BACK_RIGHT;
    mixBinVolumePair[5].lVolume = DSBVOLUME_MIN;

	DSMIXBINS mixBins;                
	mixBins.dwMixBinCount = 6;
    mixBins.lpMixBinVolumePairs = mixBinVolumePair;
	directSoundStream->SetMixBins(&mixBins);

	uint32_t* packetStatus = (uint32_t*)malloc(AUDIO_PACKETS * sizeof(uint32_t));
	memset(packetStatus, 0, AUDIO_PACKETS * sizeof(uint32_t));
	uint32_t* completedSize = (uint32_t*)malloc(AUDIO_PACKETS * sizeof(uint32_t));
	memset(completedSize, 0, AUDIO_PACKETS * sizeof(uint32_t));

	char* decodeBuffer = (char *)malloc(AUDIO_PACKETS * AUDIO_OUTPUT_BUF_SIZE);

	utils::debugPrint("Sound %i play loop begin\n", container->id);

	bool eof = false;
	while (true)
	{
		EnterCriticalSection(&mPlayerMutex);
		bool requestStop = container->requestStop;
		LeaveCriticalSection(&mPlayerMutex);

		if (mPause == true)
		{
			DirectSoundDoWork();
			Sleep(10);
			continue;
		}

		if (eof == true || requestStop == true)
		{
			DirectSoundDoWork();
			Sleep(10);
			break;
		}

		float percent = repeat ? (settingsManager::getMusicVolume() / 100.0f) : (settingsManager::getSoundVolume() / 100.0f);
		int volume = (int)(DSBVOLUME_HW_MIN * (1.0f - min(max(percent, 0.0f), 1.0f)));
		directSoundStream->SetVolume(volume);

		for (int i = 0; i < AUDIO_PACKETS; i++)
		{
			if (packetStatus[i] != XMEDIAPACKET_STATUS_PENDING)
			{
				char *decodeBufferOffset = decodeBuffer + (i * AUDIO_OUTPUT_BUF_SIZE);
				const int bytesRead = (int)stb_vorbis_get_samples_short_interleaved(vorbis, vorbisInfo.channels, (short*)decodeBufferOffset, AUDIO_OUTPUT_BUF_SIZE / sizeof(short)) * vorbisInfo.channels * sizeof(short);
				if (bytesRead != AUDIO_OUTPUT_BUF_SIZE)
				{
					memset(decodeBufferOffset + bytesRead, 0, AUDIO_OUTPUT_BUF_SIZE - bytesRead);
				}
				if (bytesRead != 0)
				{
					XMEDIAPACKET mediaPacket;
					memset(&mediaPacket, 0, sizeof(mediaPacket));
					mediaPacket.pvBuffer = decodeBufferOffset;
					mediaPacket.dwMaxSize = bytesRead;
					mediaPacket.pdwCompletedSize = &completedSize[i];
					mediaPacket.pdwStatus = &packetStatus[i];
 					directSoundStream->Process(&mediaPacket, NULL);
				}
				else if (repeat == true) 
				{
					stb_vorbis_seek(vorbis, 0);
				} 
				else 
				{
					eof = true;
					break;
				}				
			}
		}
		DirectSoundDoWork();
		Sleep(100);
	}	

	int maxSamples = ((AUDIO_PACKETS * AUDIO_OUTPUT_BUF_SIZE) / sizeof(short)) / vorbisInfo.channels;
	int waitTime = (int)(maxSamples / (vorbisInfo.sample_rate / 1000.0f));
	Sleep(waitTime);

	directSoundStream->FlushEx(0, DSSTREAMFLUSHEX_ASYNC);
	directSoundStream->Release();

	free(packetStatus);
	free(completedSize);
	free(decodeBuffer);
	stb_vorbis_close(vorbis);

	utils::debugPrint("Sound %i ending\n", container->id);

	mAudioContainerMap->removeKey(container->id);

	return 1;
}
