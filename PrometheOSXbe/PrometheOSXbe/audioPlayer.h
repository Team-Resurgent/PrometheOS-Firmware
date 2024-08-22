#pragma once

#include "xboxInternals.h"
#include "utils.h"

typedef struct audioContainer 
{	
	uint32_t id;
	char* filePath;
	bool repeat;
	bool requestStop;
	HANDLE thread;

	~audioContainer()
	{
		utils::debugPrint("Sound %i disposing\n", id);
		CloseHandle(thread);
		free(filePath);
	}
} audioContainer;

class audioPlayer
{		
public:
	static bool init();
	static bool close();
	static uint32_t play(const char* soundName, bool repeat);
	static bool stop(uint32_t key);
	static void pause(bool value);
private:
	static uint64_t WINAPI process(void* param);
};
