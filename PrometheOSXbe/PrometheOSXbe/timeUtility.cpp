#include "timeUtility.h"
#include "xboxInternals.h"

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