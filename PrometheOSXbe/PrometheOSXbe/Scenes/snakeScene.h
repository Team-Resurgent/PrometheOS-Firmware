#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class snakeScene : public scene
{
public:
	enum snakeDirectionEnum
	{
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
	void restart();
private:
	int32_t mWidth; 
	int32_t mHeight; 
	int32_t mX;
	int32_t mY;  
	int32_t mFruitCordX;
	int32_t mFruitCordY; 
	uint32_t mPlayerScore; 
	int32_t mSnakeTailX[100];
	int32_t mSnakeTailY[100]; 
	int32_t mSnakeTailLen; 
	snakeDirectionEnum mInputDirection; 
	snakeDirectionEnum mDirection; 
	bool mIsGameOver; 
	int32_t mCounter;
};
