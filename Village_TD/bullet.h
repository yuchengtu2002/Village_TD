#pragma once

#include "vector2.h"
#include "animation.h"

class Bullet {

public:
	Bullet() {

	}

	~Bullet() {

	}

protected:
	Vector2 size;
	Vector2 position;
	Vector2 velocity;

	Animation animation;



};