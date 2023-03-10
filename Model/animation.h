#ifndef ANIMATION_H
#define ANIMATION_H

#include <iostream>
#include <vector>
#include <string>

class Animation {
public:
	bool running = false;
	bool finished = true;
	bool inverseMove = false;
	int targetSide = 0;
	int axis = 0;
	float animationStartTime = 0.0f;
	float duration = 0.0f;
	float animationStep = 0.1f;

	Animation(bool running) {
		this->running = running;
		if (running)
			finished = false;
	};

	void updateAnimation(float currentTime) {
		if (!running)
			return;

		animationStep = (currentTime - animationStartTime) / duration;
		animationStep = sqrtf(animationStep);
		if (animationStep > 1.0f) {
			running = false;
			finished = true;
		}
	}

	void doAnimation(int newTarget, float animationDuration, int axis, bool inverseMove, float startTime) {
		if (running)
			return;

		targetSide = newTarget;
		this->axis = axis;
		this->inverseMove = inverseMove;
		animationStartTime = startTime;
		duration = animationDuration;
		running = true;
		finished = false;
		animationStep = 0.0f;
	}
};

#endif