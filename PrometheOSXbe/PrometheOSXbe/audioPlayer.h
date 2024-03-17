#pragma once

#include "xboxInternals.h"

class audioPlayer
{		
public:

	typedef struct audioPlayerData {
		char* filePath;
		bool repeat;
		bool requestStop;
		HANDLE thread;
	} audioPlayerData;

	typedef struct audioContainer 
	{	
		audioPlayer::audioPlayerData* audioPlayerData;
		~audioContainer()
		{
			free(audioPlayerData->filePath);
			free(audioPlayerData);
		}
	} audioContainer;

	static bool init();
	static bool close();
	static uint32_t play(const char* soundName, bool repeat);
	static bool stop(uint32_t key);
	static void pause(bool value);
	static void refresh();
private:
	static uint64_t WINAPI process(void* param);
};
