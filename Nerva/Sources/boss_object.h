#pragma once

#include "model_object.h"
#include "collision.h"

class BossObject : public ModelObject
{
public:
	int move_type = 0;
	void update(std::shared_ptr<Collision> collision, std::shared_ptr<ModelObject> model_object);
};
