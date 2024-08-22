#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class invadersScene : public scene
{
public:

	typedef struct Invader
	{
		int invaderX;
		int invaderY;
		int invaderScore;
		bool invaderDestroyed;
		int bombX;
		int bombY;
		bool bombDestroyed;
	} Invader;

	invadersScene();
	void update();
	void render();
private:
	void restart(int32_t level);
private:
	int32_t mWidth; 
	int32_t mHeight; 
	uint32_t mPlayerScore; 

	int32_t mBulletX;
	int32_t mBulletY;
	bool  mBulletDestroyed;

	int32_t mBossX;
	int32_t mBossY;
	bool mBossDestroyed;

	int32_t mPlayerX;
	int32_t mPlayerY;
	int32_t mInvaderDirection;
	Invader mInvaders[44];

	bool mIsGameOver; 
	int32_t mLevel;
	int32_t mCounter;
};
