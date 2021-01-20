#pragma once

#include "model_object.h"

class PlayerObject : public ModelObject
{
private:
	DirectX::XMFLOAT3 velocity;
public:
	void move();

	void update();
};
