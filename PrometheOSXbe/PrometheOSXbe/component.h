#pragma once

#include "xboxInternals.h"
#include "alignment.h"

#include <string>

class component
{
public:
	static void panel(uint32_t fill, uint32_t stroke, int x, int y, int width, int height);
	static void button(bool selected, bool active, const char* label, int x, int y, int width, int height);
	static void splitButton(bool selected, bool active, const char* label1, int label1Width, const char* label2, int x, int y, int width, int height);
	static void fileButton(bool selected, bool active, bool isFile, const char* label, int x, int y, int width, int height);
	static void ledButton(bool selected, uint8_t ledColor, const char* label, int x, int y, int width, int height);
	static void text(const char* label, bool disabled, horizAlignment hAlign, int x, int y, int width, int height);
	static void textBox(const char* label, bool selected, bool disabled, horizAlignment hAlign, int x, int y, int width, int height, bool vAlignTop = false, bool solidBg = false);
};
