#include "xmvPlayer.h"
#include "context.h"

#include <xtl.h>
#include <xmv.h>

#define XMVPLAYER_NUMTEXTURES 2

namespace {
XMVDecoder* mXMVDecoder;
XMVVIDEO_DESC mVideoDesc;
XMVAUDIO_DESC mAudioDesc;
LPDIRECT3DTEXTURE8 mTextures[XMVPLAYER_NUMTEXTURES];
DWORD mDecodeTextureIndex;
LPDIRECT3DTEXTURE8 mShowingTexture;
LPDIRECT3DTEXTURE8 mDecodingTexture;
LPDIRECT3DTEXTURE8 mSubmittedTexture;
DWORD mCurrentFrame;
bool mPlaying;
bool mPause;
} // namespace

void xmvPlayer::init() {
    mPlaying = false;
    mPause = false;
    mXMVDecoder = NULL;

    ZeroMemory(&mVideoDesc, sizeof(mVideoDesc));
    ZeroMemory(&mAudioDesc, sizeof(mAudioDesc));
    for (UINT i = 0; i < XMVPLAYER_NUMTEXTURES; i++) {
        mTextures[i] = NULL;
    }
}

void xmvPlayer::play(const char* path) {
    if (mPlaying == true) {
        close();
    }

    mCurrentFrame = -1;

    HRESULT hr = XMVDecoder_CreateDecoderForFile(XMVFLAG_FULL_LOOP, path, &mXMVDecoder);
    if (FAILED(hr)) {
        return;
    }

    mXMVDecoder->GetVideoDescriptor(&mVideoDesc);

    for (uint32_t i = 0; i < mVideoDesc.AudioStreamCount; i++) {
        mXMVDecoder->GetAudioDescriptor(i, &mAudioDesc);
        hr = mXMVDecoder->EnableAudioStream(i, 0, NULL, NULL);
        if (FAILED(hr)) {
            close();
            return;
        }
    }

    for (int i = 0; i < XMVPLAYER_NUMTEXTURES; i++) {
        mTextures[i] = 0;
        hr = context::getD3dDevice()->CreateTexture(
            mVideoDesc.Width, mVideoDesc.Height, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &mTextures[i]);
        if (FAILED(hr)) {
            close();
            return;
        }
    }

    mDecodeTextureIndex = 0;
    mShowingTexture = mTextures[0];
    mDecodingTexture = mTextures[1];
    mSubmittedTexture = 0;
    mPlaying = true;
}

uint32_t xmvPlayer::getWidth() {
    return mVideoDesc.Width;
}

uint32_t xmvPlayer::getHeight() {
    return mVideoDesc.Height;
}

void xmvPlayer::close() {
    if (mXMVDecoder != NULL) {
        mXMVDecoder->CloseDecoder();
        mXMVDecoder = NULL;
    }

    for (UINT i = 0; i < XMVPLAYER_NUMTEXTURES; i++) {
        if (mTextures[i] != NULL) {
            mTextures[i]->Release();
        }
        mTextures[i] = NULL;
    }

    mPlaying = false;
}

void xmvPlayer::pause(bool value) {
    mPause = value;
}

LPDIRECT3DTEXTURE8 xmvPlayer::advanceFrameForTexturing() {
    if (mPlaying == false || mPause == true) {
        return NULL;
    }

    LPDIRECT3DSURFACE8 pSurface;
    mDecodingTexture->GetSurfaceLevel(0, &pSurface);

    XMVRESULT xr = XMV_NOFRAME;
    mXMVDecoder->GetNextFrame(pSurface, &xr, NULL);

    switch (xr) {
        case XMV_NOFRAME:
            break;
        case XMV_NEWFRAME:
            mCurrentFrame++;
            mShowingTexture = mDecodingTexture;
            mDecodeTextureIndex = (mDecodeTextureIndex + 1) % XMVPLAYER_NUMTEXTURES;
            mDecodingTexture = mTextures[mDecodeTextureIndex];
            break;
        case XMV_ENDOFFILE:
            mPlaying = false;
            break;
        case XMV_FAIL:
            mPlaying = false;
            break;
    }

    if (pSurface != NULL) {
        pSurface->Release();
        pSurface = NULL;
    }

    if (mCurrentFrame < 0) {
        return NULL;
    }

    return mShowingTexture;
}
