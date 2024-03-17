#include "math.h"

float math::calcNinePatchPosition(int index, float length)
{
	if (index == 0)
	{
		return 0.0f;
	}
	if (index == 1)
	{
		return 8.0f;
	}
	if (index == 2)
	{
		return length - 8.0f;
	}
	return length;
}

float math::calcNinePatchUV(int index, float length)
{
	float uvPerPixel = length / 24.0f;
	if (index == 0)
	{
		return 0.0f;
	}
	if (index == 1)
	{
		return 8.0f * uvPerPixel;
	}
	if (index == 2)
	{
		return length - (8.0f * uvPerPixel);
	}
	return length;
}