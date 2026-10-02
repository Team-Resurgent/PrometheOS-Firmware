#pragma once

#include "xboxinternals.h"

#include <xtl.h>

class xmvPlayer {
  public:
    static void init();
    static void play(const char* path);
    static uint32_t getWidth();
    static uint32_t getHeight();
    static void close();
    static void pause(bool value);
    static LPDIRECT3DTEXTURE8 advanceFrameForTexturing();
};
