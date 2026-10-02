#include "snakeScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"

snakeScene::snakeScene() {
    mCounter = 0;
    restart();
}

void snakeScene::readPlayerInput(snakePlayer* player, int playerIndex) {
    if (player->isGameOver || isPlayerActive(playerIndex) == false) {
        return;
    }

    int port = inputManager::getConnectedPort(playerIndex);
    if (port < 0) {
        return;
    }

    if (inputManager::buttonPressed(JoystickButtonDpadLeft, port) && player->direction != snakeDirectionRight) {
        player->inputDirection = snakeDirectionLeft;
    } else if (inputManager::buttonPressed(JoystickButtonDpadRight, port) && player->direction != snakeDirectionLeft) {
        player->inputDirection = snakeDirectionRight;
    } else if (inputManager::buttonPressed(JoystickButtonDpadDown, port) && player->direction != snakeDirectionUp) {
        player->inputDirection = snakeDirectionDown;
    } else if (inputManager::buttonPressed(JoystickButtonDpadUp, port) && player->direction != snakeDirectionDown) {
        player->inputDirection = snakeDirectionUp;
    }
}

void snakeScene::movePlayer(snakePlayer* player) {
    if (player->isGameOver) {
        return;
    }

    player->direction = player->inputDirection;

    int prevX = player->tailX[0];
    int prevY = player->tailY[0];
    int prev2X, prev2Y;
    player->tailX[0] = player->x;
    player->tailY[0] = player->y;
    for (int i = 1; i < player->tailLen; i++) {
        prev2X = player->tailX[i];
        prev2Y = player->tailY[i];
        player->tailX[i] = prevX;
        player->tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (player->direction) {
        case snakeDirectionLeft:
            player->x--;
            break;
        case snakeDirectionRight:
            player->x++;
            break;
        case snakeDirectionUp:
            player->y--;
            break;
        case snakeDirectionDown:
            player->y++;
            break;
        default:
            break;
    }
}

void snakeScene::checkPlayerCollisions(snakePlayer* player, snakePlayer* other) {
    if (player->isGameOver) {
        return;
    }

    if (player->x >= mWidth || player->x < 0 || player->y >= mHeight || player->y < 0) {
        player->isGameOver = true;
        return;
    }

    for (int i = 0; i < player->tailLen; i++) {
        if (player->tailX[i] == player->x && player->tailY[i] == player->y) {
            player->isGameOver = true;
            return;
        }
    }

    if (other != NULL && other != player) {
        if (other->x == player->x && other->y == player->y) {
            player->isGameOver = true;
            return;
        }

        for (int i = 0; i < other->tailLen; i++) {
            if (other->tailX[i] == player->x && other->tailY[i] == player->y) {
                player->isGameOver = true;
                return;
            }
        }
    }
}

bool snakeScene::isPlayerActive(int playerIndex) {
    return inputManager::getConnectedPort(playerIndex) >= 0;
}

bool snakeScene::areAllPlayersDead() {
    bool p1Active = isPlayerActive(0);
    bool p2Active = isPlayerActive(1);

    if (p1Active && mPlayers[0].isGameOver == false) {
        return false;
    }
    if (p2Active && mPlayers[1].isGameOver == false) {
        return false;
    }
    return true;
}

void snakeScene::update() {
    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene();
        return;
    }

    if (areAllPlayersDead() && inputManager::buttonPressed(JoystickButtonA)) {
        restart();
        return;
    }

    if (areAllPlayersDead()) {
        return;
    }

    readPlayerInput(&mPlayers[0], 0);
    readPlayerInput(&mPlayers[1], 1);

    mCounter++;
    if (mCounter < 5) {
        return;
    }
    mCounter = 0;

    if (isPlayerActive(0)) {
        movePlayer(&mPlayers[0]);
    }
    if (isPlayerActive(1)) {
        movePlayer(&mPlayers[1]);
    }

    if (isPlayerActive(0) && isPlayerActive(1)) {
        checkPlayerCollisions(&mPlayers[0], &mPlayers[1]);
        checkPlayerCollisions(&mPlayers[1], &mPlayers[0]);
    } else if (isPlayerActive(0)) {
        checkPlayerCollisions(&mPlayers[0], NULL);
    } else if (isPlayerActive(1)) {
        checkPlayerCollisions(&mPlayers[1], NULL);
    }

    for (int p = 0; p < 2; p++) {
        if (isPlayerActive(p) == false) {
            continue;
        }
        snakePlayer* player = &mPlayers[p];
        if (player->isGameOver) {
            continue;
        }

        if (player->x == mFruitCordX && player->y == mFruitCordY) {
            player->score += 10;
            mFruitCordX = rand() % mWidth;
            mFruitCordY = rand() % mHeight;
            if (player->tailLen < 100) {
                player->tailLen++;
            }
        }
    }

    if (areAllPlayersDead()) {
        uint32_t bestScore = 0;
        if (isPlayerActive(0)) {
            bestScore = max(bestScore, mPlayers[0].score);
        }
        if (isPlayerActive(1)) {
            bestScore = max(bestScore, mPlayers[1].score);
        }
        if (bestScore > settingsManager::getSnakeHiScore()) {
            settingsManager::setSnakeHiScore(bestScore);
        }
    }
}

void snakeScene::render() {
    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Snake", theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    int xPos = (context::getBufferWidth() - ((mWidth + 2) * 10)) / 2;
    int yPos = (context::getBufferHeight() - ((mHeight + 2) * 10)) / 2;
    yPos += theme::getCenterOffset();

    for (int i = 0; i < mWidth + 2; i++) {
        drawing::drawNinePatch("panel-fill", theme::getSnakeWallColor(), xPos + (i * 10), yPos, 10, 10);
        drawing::drawNinePatch(
            "panel-fill", theme::getSnakeWallColor(), xPos + (i * 10), yPos + (mHeight + 1) * 10, 10, 10);
    }

    for (int i = 0; i < mHeight; i++) {
        drawing::drawNinePatch("panel-fill", theme::getSnakeWallColor(), xPos, yPos + ((i + 1) * 10), 10, 10);
        drawing::drawNinePatch(
            "panel-fill", theme::getSnakeWallColor(), xPos + ((mWidth + 1) * 10), yPos + ((i + 1) * 10), 10, 10);
    }

    uint32_t headColors[2] = {theme::getSnakeHeadColor(), theme::getSnakeHead2Color()};
    uint32_t tailColors[2] = {theme::getSnakeTailColor(), theme::getSnakeTail2Color()};

    for (int i = 0; i < mHeight; i++) {
        for (int j = 0; j <= mWidth; j++) {
            int screenPosX = xPos + ((j + 1) * 10);
            int screenPosY = yPos + ((i + 1) * 10);
            bool drawn = false;

            if (i == mFruitCordY && j == mFruitCordX) {
                drawing::drawNinePatch("panel-fill", theme::getSnakeFoodColor(), screenPosX, screenPosY, 10, 10);
                drawn = true;
            }

            for (int p = 0; p < 2 && drawn == false; p++) {
                if (isPlayerActive(p) == false) {
                    continue;
                }
                snakePlayer* player = &mPlayers[p];
                if (player->isGameOver) {
                    continue;
                }

                if (i == player->y && j == player->x) {
                    drawing::drawNinePatch("panel-fill", headColors[p], screenPosX, screenPosY, 10, 10);
                    drawn = true;
                    break;
                }

                for (int k = 0; k < player->tailLen; k++) {
                    if (player->tailX[k] == j && player->tailY[k] == i) {
                        drawing::drawNinePatch("panel-fill", tailColors[p], screenPosX, screenPosY, 10, 10);
                        drawn = true;
                        break;
                    }
                }
            }
        }
    }

#ifndef TOOLS
    char* score = stringUtility::formatString("Hi-Score: %i, P1: %i, P2: %i", settingsManager::getSnakeHiScore(),
        mPlayers[0].score, mPlayers[1].score);
#else
    char* score = stringUtility::formatString("P1: %i, P2: %i", mPlayers[0].score, mPlayers[1].score);
#endif

    if (areAllPlayersDead()) {
        int yPosGameOver = (context::getBufferHeight() - 20) / 2;
        yPosGameOver += theme::getCenterOffset();
        drawing::drawBitmapStringAligned(context::getBitmapFontLarge(), "GAME OVER", theme::getTextColor(),
            horizAlignmentCenter, 40, yPosGameOver, 640);
        drawing::drawBitmapString(
            context::getBitmapFontSmall(), "\xC2\xA1 Restart", theme::getFooterTextColor(), 40, theme::getFooterY());
    } else if ((isPlayerActive(0) && mPlayers[0].isGameOver) || (isPlayerActive(1) && mPlayers[1].isGameOver)) {
        char* status = stringUtility::formatString("P%i OUT", (isPlayerActive(0) && mPlayers[0].isGameOver) ? 1 : 2);
        drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), status, theme::getFooterTextColor(),
            horizAlignmentLeft, 40, theme::getFooterY() - 22, 640);
        free(status);
    }

    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), score, theme::getFooterTextColor(),
        horizAlignmentCenter, 40, theme::getFooterY(), 640);
    free(score);
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(),
        horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void snakeScene::restartPlayer(snakePlayer* player, int32_t startX, int32_t startY) {
    player->x = startX;
    player->y = startY;
    player->tailLen = 0;
    player->isGameOver = false;
    player->inputDirection = snakeDirectionStop;
    player->direction = snakeDirectionStop;
    player->score = 0;
    for (int i = 0; i < 100; i++) {
        player->tailX[i] = -1;
        player->tailY[i] = -1;
    }
}

void snakeScene::restart() {
    mWidth = 58;
    mHeight = 26;
    mCounter = 0;
    mFruitCordX = rand() % mWidth;
    mFruitCordY = rand() % mHeight;
    restartPlayer(&mPlayers[0], mWidth / 4, mHeight / 2);
    restartPlayer(&mPlayers[1], (mWidth / 4) * 3, mHeight / 2);
}
