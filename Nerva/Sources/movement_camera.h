#pragma once

#include "camera.h"

class MovementCamera : public Camera
{
public:
	MovementCamera();
	void update();
};
