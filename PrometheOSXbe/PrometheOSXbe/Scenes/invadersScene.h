#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class invadersScene : public scene {
  public:
    typedef struct Invader {
        int invaderX;
        int invaderY;
        int invaderScore;
        bool invaderDestroyed;
        int bombX;
        int bombY;
        bool bombDestroyed;
    } Invader;

    typedef struct invadersPlayer {
        int32_t x;
        int32_t y;
        int32_t bulletX;
        int32_t bulletY;
        bool bulletDestroyed;
    } invadersPlayer;

    invadersScene();
    void update();
    void render();

  private:
    void restart(int32_t level);
    void updatePlayer(invadersPlayer* player, int playerIndex);
    void moveBullet(invadersPlayer* player);
    void checkBulletHits(invadersPlayer* player);
    void checkPlayerHit(invadersPlayer* player);
    bool isPlayerActive(int playerIndex);

    int32_t mWidth;
    int32_t mHeight;
    uint32_t mPlayerScore;

    invadersPlayer mPlayers[2];

    int32_t mBossX;
    int32_t mBossY;
    bool mBossDestroyed;

    int32_t mInvaderDirection;
    Invader mInvaders[44];

    bool mIsGameOver;
    int32_t mLevel;
    int32_t mCounter;
};
