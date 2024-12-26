#include "snakeScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"

snakeScene::snakeScene()
{
	mCounter = 0;
	restart();
}

void snakeScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	//// Select Actions

	if (mIsGameOver == true && inputManager::buttonPressed(ButtonA))
	{
		restart();
	}

	if (mIsGameOver == true)
	{
		 return;
	}

	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft) && mDirection != snakeDirectionRight)
	{
		mInputDirection = snakeDirectionLeft;
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight) && mDirection != snakeDirectionLeft)
	{
		mInputDirection = snakeDirectionRight;
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown) && mDirection != snakeDirectionUp)
	{
		mInputDirection = snakeDirectionDown;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp) && mDirection != snakeDirectionDown)
	{
		mInputDirection = snakeDirectionUp;
	}

	mCounter++;
	if (mCounter < 5)
	{
		return;
	}
	mCounter = 0;

	mDirection = mInputDirection;

	int prevX = mSnakeTailX[0]; 
    int prevY = mSnakeTailY[0]; 
    int prev2X, prev2Y; 
    mSnakeTailX[0] = mX; 
    mSnakeTailY[0] = mY; 
    for (int i = 1; i < mSnakeTailLen; i++) { 
        prev2X = mSnakeTailX[i]; 
        prev2Y = mSnakeTailY[i]; 
        mSnakeTailX[i] = prevX; 
        mSnakeTailY[i] = prevY; 
        prevX = prev2X; 
        prevY = prev2Y; 
    } 
  
    switch (mDirection) 
	{ 
	case snakeDirectionLeft: 
        mX--; 
        break; 
    case snakeDirectionRight: 
        mX++; 
        break; 
    case snakeDirectionUp: 
        mY--; 
        break; 
    case snakeDirectionDown: 
        mY++; 
        break; 
    } 
  
    if (mX >= mWidth || mX < 0 || mY >= mHeight || mY < 0) 
	{
        mIsGameOver = true; 
	}

    for (int i = 0; i < mSnakeTailLen; i++) 
	{ 
        if (mSnakeTailX[i] == mX && mSnakeTailY[i] == mY) 
		{
            mIsGameOver = true; 
		}
    } 

	if (mIsGameOver == true && mPlayerScore > settingsManager::getSnakeHiScore())
	{
		settingsManager::setSnakeHiScore(mPlayerScore);
	}

    if (mX == mFruitCordX && mY == mFruitCordY) 
	{ 
        mPlayerScore += 10; 
        mFruitCordX = rand() % mWidth; 
        mFruitCordY = rand() % mHeight;
		if (mSnakeTailLen < 100)
		{
			mSnakeTailLen++;
		}
    } 
}

void snakeScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Snake", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int xPos = (context::getBufferWidth() - ((mWidth + 2) * 10)) / 2;
	int yPos = (context::getBufferHeight() - ((mHeight + 2) * 10)) / 2;
	yPos += theme::getCenterOffset();

	for (int i = 0; i < mWidth + 2; i++) 
	{
		drawing::drawNinePatch("panel-fill", theme::getSnakeWallColor(), xPos + (i * 10), yPos, 10, 10);
		drawing::drawNinePatch("panel-fill", theme::getSnakeWallColor(), xPos + (i * 10), yPos + (mHeight + 1) * 10, 10, 10);
	}

	for (int i = 0; i < mHeight; i++) 
	{ 
		drawing::drawNinePatch("panel-fill", theme::getSnakeWallColor(), xPos, yPos + ((i + 1) * 10), 10, 10);
		drawing::drawNinePatch("panel-fill", theme::getSnakeWallColor(), xPos + ((mWidth + 1) * 10), yPos + ((i + 1) * 10), 10, 10);
	}

    for (int i = 0; i < mHeight; i++) { 
        for (int j = 0; j <= mWidth; j++) { 

			int screenPosX = xPos + ((j + 1) * 10);
			int screenPosY = yPos + ((i + 1) * 10);

            if (i == mY && j == mX) 
			{  
				drawing::drawNinePatch("panel-fill", theme::getSnakeHeadColor(), screenPosX, screenPosY, 10, 10);
			}
			else if (i == mFruitCordY && j == mFruitCordX) 
			{
				drawing::drawNinePatch("panel-fill", theme::getSnakeFoodColor(), screenPosX, screenPosY, 10, 10);
			}
			else 
			{ 
                for (int k = 0; k < mSnakeTailLen; k++) 
				{ 
                    if (mSnakeTailX[k] == j  && mSnakeTailY[k] == i) 
					{ 
						drawing::drawNinePatch("panel-fill", theme::getSnakeTailColor(), screenPosX, screenPosY, 10, 10);
                    } 
                } 
            } 
        } 
    } 

#ifndef TOOLS
	char* score = stringUtility::formatString("Hi-Score: %i, Score: %i", settingsManager::getSnakeHiScore(), mPlayerScore);
#else
	char* score = stringUtility::formatString("Score: %i", mPlayerScore);
#endif

	if (mIsGameOver == true)
	{
		int yPosGameOver = (context::getBufferHeight() - 20) / 2;
		yPosGameOver += theme::getCenterOffset();
		drawing::drawBitmapStringAligned(context::getBitmapFontLarge(), "GAME OVER", theme::getTextColor(), horizAlignmentCenter, 40, yPosGameOver, 640);
		drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Restart", theme::getFooterTextColor(), 40, theme::getFooterY());
	}

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), score, theme::getFooterTextColor(), horizAlignmentCenter, 40, theme::getFooterY(), 640);
	free(score);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void snakeScene::restart()
{
	mWidth = 58; 
	mHeight = 26; 
	mSnakeTailLen = 0;
	mIsGameOver = false; 
	mInputDirection = snakeDirectionStop; 
    mDirection = snakeDirectionStop; 
    mX = mWidth / 2; 
    mY = mHeight / 2; 
    mFruitCordX = rand() % mWidth; 
    mFruitCordY = rand() % mHeight; 
    mPlayerScore = 0; 
	for (int i = 0; i < 100; i++)
	{
		mSnakeTailX[i] = -1;
		mSnakeTailY[i] = -1;
	}
}