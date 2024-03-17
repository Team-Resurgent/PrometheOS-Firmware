#pragma once

enum sceneResult
{
	sceneResultNone = 0,
	sceneResultDone = 1,
	sceneResultCancelled = 2
};

class scene 
{
public:
	virtual void update() = 0;
	virtual void render() = 0;
};
