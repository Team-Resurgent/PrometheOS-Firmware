#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class snakeScene : public scene {
  public:
    enum snakeDirectionEnum {
        snakeDirectionStop = 0,
        snakeDirectionLeft = 1,
        snakeDirectionRight = 2,
        snakeDirectionUp = 3,
        snakeDirectionDown = 4
    };

    snakeScene();
    void update();
    void render();

  private:
    typedef struct snakePlayer {
        int32_t x;
        int32_t y;
        int32_t tailX[100];
        int32_t tailY[100];
        int32_t tailLen;
        snakeDirectionEnum inputDirection;
        snakeDirectionEnum direction;
        uint32_t score;
        bool isGameOver;
    } snakePlayer;

    void restart();
    void restartPlayer(snakePlayer* player, int32_t startX, int32_t startY);
    void readPlayerInput(snakePlayer* player, int playerIndex);
    void movePlayer(snakePlayer* player);
    void checkPlayerCollisions(snakePlayer* player, snakePlayer* other);
    bool isPlayerActive(int playerIndex);
    bool areAllPlayersDead();

    int32_t mWidth;
    int32_t mHeight;
    int32_t mFruitCordX;
    int32_t mFruitCordY;
    snakePlayer mPlayers[2];
    int32_t mCounter;
};
