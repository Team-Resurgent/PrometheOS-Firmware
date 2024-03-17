#include "timeUtility.h"
#include "xboxInternals.h"
#include <time.h>

uint64_t timeUtility::getMillisecondsNow()
{
	return (uint64_t)GetTickCount();
}

double timeUtility::getDurationSeconds(uint64_t start, uint64_t end)
{
	return (end - start) / (double)1000;
}

double timeUtility::calculateFramesPerSecond(uint32_t frameCount, double duration)
{
	return frameCount / duration;
}

char* timeUtility::getTimeString()
{
	time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", timeinfo);
    return strdup(buffer);
}