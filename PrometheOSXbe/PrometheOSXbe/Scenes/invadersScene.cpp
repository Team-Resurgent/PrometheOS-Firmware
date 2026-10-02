#include "invadersScene.h"
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
#include "..\timeUtility.h"

invadersScene::invadersScene() {
    mCounter = 0;
    mPlayerScore = 0;
    restart(0);
}

void invadersScene::updatePlayer(invadersPlayer* player, int playerIndex) {
    if (isPlayerActive(playerIndex) == false) {
        return;
    }

    int port = inputManager::getConnectedPort(playerIndex);
    if (port < 0) {
        return;
    }

    if (inputManager::buttonDown(JoystickButtonDpadLeft, port) && player->x > 0) {
        player->x -= 5;
    }

    if (inputManager::buttonDown(JoystickButtonDpadRight, port) && player->x < ((mWidth * 10) - 20)) {
        player->x += 5;
    }

    if (inputManager::buttonPressed(JoystickButtonA, port) && player->bulletDestroyed == true) {
        player->bulletX = player->x + 5;
        player->bulletY = (mHeight - 2) * 10;
        player->bulletDestroyed = false;
    }
}

void invadersScene::moveBullet(invadersPlayer* player) {
    if (player->bulletDestroyed == true) {
        return;
    }

    player->bulletY -= 5;
    if (player->bulletY < 0) {
        player->bulletDestroyed = true;
    }
}

void invadersScene::checkBulletHits(invadersPlayer* player) {
    if (player->bulletDestroyed == true) {
        return;
    }

    for (int i = 0; i < 44; i++) {
        Invader* invader = &mInvaders[i];
        if (invader->invaderDestroyed == true) {
            continue;
        }

        if ((player->bulletX + 10) > invader->invaderX && player->bulletX < (invader->invaderX + 20)) {
            if ((player->bulletY + 10) > invader->invaderY && player->bulletY < (invader->invaderY + 20)) {
                invader->invaderDestroyed = true;
                mPlayerScore += invader->invaderScore;
                player->bulletDestroyed = true;
                return;
            }
        }
    }

    if (mBossDestroyed == false) {
        if ((player->bulletX + 10) > mBossX && player->bulletX < (mBossX + 20)) {
            if ((player->bulletY + 10) > mBossY && player->bulletY < (mBossY + 10)) {
                mBossDestroyed = true;
                mPlayerScore += 100;
                player->bulletDestroyed = true;
            }
        }
    }
}

void invadersScene::checkPlayerHit(invadersPlayer* player) {
    for (int i = 0; i < 44; i++) {
        Invader* invader = &mInvaders[i];
        if (invader->bombDestroyed == true) {
            continue;
        }

        if ((invader->bombX + 10) > player->x && invader->bombX < (player->x + 20)) {
            if ((invader->bombY + 10) > player->y && invader->bombY < (player->y + 10)) {
                mIsGameOver = true;
                return;
            }
        }
    }
}

bool invadersScene::isPlayerActive(int playerIndex) {
    return inputManager::getConnectedPort(playerIndex) >= 0;
}

void invadersScene::update() {
    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene();
        return;
    }

    if (mIsGameOver == true) {
        if (inputManager::buttonPressed(JoystickButtonA)) {
            restart(0);
        }
        return;
    }

    updatePlayer(&mPlayers[0], 0);
    updatePlayer(&mPlayers[1], 1);

    if (isPlayerActive(0)) {
        moveBullet(&mPlayers[0]);
        checkBulletHits(&mPlayers[0]);
        checkPlayerHit(&mPlayers[0]);
    }
    if (isPlayerActive(1)) {
        moveBullet(&mPlayers[1]);
        checkBulletHits(&mPlayers[1]);
        checkPlayerHit(&mPlayers[1]);
    }

    int totalInvaders = 0;
    for (int i = 0; i < 44; i++) {
        if (mInvaders[i].invaderDestroyed == false) {
            totalInvaders++;
        }
    }

    if (totalInvaders == 0 && mBossDestroyed == true) {
        restart(mLevel + 1);
    }

    int gameSpeed = 5 - min(mLevel / 2, 4);

    for (int i = 0; i < 44; i++) {
        Invader* invader = &mInvaders[i];
        if (invader->bombDestroyed == true) {
            continue;
        }
        invader->bombY += 5;
        if (invader->bombY >= ((mHeight * 10) - 10)) {
            invader->bombDestroyed = true;
        }
    }

    if (mBossDestroyed == false) {
        mBossX += 2;
        if (mBossX >= ((mWidth * 10) - 20)) {
            mBossDestroyed = true;
        }
    }

    if (mIsGameOver == true && mPlayerScore > settingsManager::getInvadersHiScore()) {
        settingsManager::setInvadersHiScore(mPlayerScore);
    }

    mCounter++;
    if (mCounter < gameSpeed) {
        return;
    }
    mCounter = 0;

    if (mBossDestroyed == true && (rand() % 500) <= mLevel) {
        mBossX = 0;
        mBossDestroyed = false;
    }

    int32_t bombCount = 0;
    for (int i = 0; i < 44; i++) {
        if (mInvaders[i].bombDestroyed == false) {
            bombCount++;
        }
    }

    bool moveDown = false;
    for (int i = 0; i < 44; i++) {
        Invader* invader = &mInvaders[i];
        if (invader->invaderDestroyed == true) {
            continue;
        }
        invader->invaderX += mInvaderDirection;

        if (invader->bombDestroyed == true) {
            if (bombCount <= mLevel && (rand() % 1000) <= mLevel) {
                invader->bombDestroyed = false;
                invader->bombX = invader->invaderX + 5;
                invader->bombY = invader->invaderY + 20;
                bombCount++;
            }
        }

        if (mInvaderDirection < 0 && invader->invaderX == 0) {
            moveDown = true;
        } else if (mInvaderDirection > 0 && invader->invaderX == ((mWidth * 10) - 20)) {
            moveDown = true;
        }
    }

    if (moveDown == true) {
        mInvaderDirection = mInvaderDirection * -1;
        for (int i = 0; i < 44; i++) {
            Invader* invader = &mInvaders[i];
            invader->invaderY += 10;
            if (invader->invaderY == ((mHeight * 10) - 30)) {
                mIsGameOver = true;
            }
        }
    }
}

void invadersScene::render() {
    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    char* title = stringUtility::formatString("Invaders - Level %i", mLevel + 1);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), title, theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
    free(title);

    int xPos = (context::getBufferWidth() - ((mWidth + 2) * 10)) / 2;
    int yPos = (context::getBufferHeight() - ((mHeight + 2) * 10)) / 2;
    yPos += theme::getCenterOffset();

    for (int i = 0; i < mWidth + 2; i++) {
        drawing::drawNinePatch("panel-fill", theme::getInvadersWallColor(), xPos + (i * 10), yPos, 10, 10);
        drawing::drawNinePatch(
            "panel-fill", theme::getInvadersWallColor(), xPos + (i * 10), yPos + (mHeight + 1) * 10, 10, 10);
    }

    for (int i = 0; i < mHeight; i++) {
        drawing::drawNinePatch("panel-fill", theme::getInvadersWallColor(), xPos, yPos + ((i + 1) * 10), 10, 10);
        drawing::drawNinePatch(
            "panel-fill", theme::getInvadersWallColor(), xPos + ((mWidth + 1) * 10), yPos + ((i + 1) * 10), 10, 10);
    }

    uint32_t playerColors[2] = {theme::getInvadersPlayerColor(), theme::getInvadersPlayer2Color()};

    for (int p = 0; p < 2; p++) {
        if (isPlayerActive(p) == false) {
            continue;
        }
        invadersPlayer* player = &mPlayers[p];
        int playerPosX = xPos + player->x + 10;
        int playerPosY = yPos + player->y + 10;
        drawing::drawNinePatch("panel-fill", playerColors[p], playerPosX, playerPosY, 20, 10);

        if (player->bulletDestroyed == false) {
            int bulletPosX = xPos + player->bulletX + 10;
            int bulletPosY = yPos + player->bulletY + 10;
            drawing::drawNinePatch("panel-fill", playerColors[p], bulletPosX, bulletPosY, 10, 10);
        }
    }

    for (int i = 0; i < 44; i++) {
        Invader* invader = &mInvaders[i];

        if (invader->invaderDestroyed == false) {
            int screenPosX = xPos + invader->invaderX + 10;
            int screenPosY = yPos + invader->invaderY + 10;
            drawing::drawNinePatch("panel-fill", theme::getInvadersAlienColor(), screenPosX, screenPosY, 20, 20);
        }

        if (invader->bombDestroyed == false) {
            int bombPosX = xPos + invader->bombX + 10;
            int bombPosY = yPos + invader->bombY + 10;
            drawing::drawNinePatch("panel-fill", theme::getInvadersAlienColor(), bombPosX, bombPosY, 10, 10);
        }
    }

    if (mBossDestroyed == false) {
        int bossPosX = xPos + mBossX + 10;
        int bossPosY = yPos + mBossY + 10;
        drawing::drawNinePatch("panel-fill", theme::getInvadersBossColor(), bossPosX, bossPosY, 20, 10);
    }

#ifndef TOOLS
    char* score =
        stringUtility::formatString("Hi-Score: %i, Score: %i", settingsManager::getInvadersHiScore(), mPlayerScore);
#else
    char* score = stringUtility::formatString("Score: %i", mPlayerScore);
#endif

    if (mIsGameOver == true) {
        int yPosGameOver = (context::getBufferHeight() - 20) / 2;
        yPosGameOver += theme::getCenterOffset();
        drawing::drawBitmapStringAligned(context::getBitmapFontLarge(), "GAME OVER", theme::getTextColor(),
            horizAlignmentCenter, 40, yPosGameOver, 640);
        drawing::drawBitmapString(
            context::getBitmapFontSmall(), "\xC2\xA1 Restart", theme::getFooterTextColor(), 40, theme::getFooterY());
    }

    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), score, theme::getFooterTextColor(),
        horizAlignmentCenter, 40, theme::getFooterY(), 640);
    free(score);
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(),
        horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void invadersScene::restart(int32_t level) {
    mWidth = 58;
    mHeight = 26;
    mIsGameOver = false;

    if (level == 0) {
        mPlayerScore = 0;
    }

    srand((uint32_t)timeUtility::getMillisecondsNow());

    mLevel = level;
    mCounter = 0;

    mBossX = 0;
    mBossY = 0;
    mBossDestroyed = true;

    mPlayers[0].x = 0;
    mPlayers[0].y = (mHeight - 1) * 10;
    mPlayers[0].bulletX = 0;
    mPlayers[0].bulletY = 0;
    mPlayers[0].bulletDestroyed = true;

    mPlayers[1].x = (mWidth * 10) - 40;
    mPlayers[1].y = (mHeight - 1) * 10;
    mPlayers[1].bulletX = 0;
    mPlayers[1].bulletY = 0;
    mPlayers[1].bulletDestroyed = true;

    mInvaderDirection = -1;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 11; x++) {
            Invader invader;
            invader.invaderX = 130 + (x * 30);
            invader.invaderY = 20 + (y * 30);
            invader.invaderScore = (5 - y) * 10;
            invader.invaderDestroyed = false;
            invader.bombX = 0;
            invader.bombY = 0;
            invader.bombDestroyed = true;
            mInvaders[(y * 11) + x] = invader;
        }
    }
}
