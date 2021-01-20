#include "../NervaLib/key_input.h"
#include "player_object.h"

void PlayerObject::update()
{
	move();

	transform.update();
	transform.calculate_world_transform_quaternion();
}

void PlayerObject::move()
{
	if (KeyInput::key_state() && KEY_UP)
	{
		velocity.z += 0.1;
	}

	if (KeyInput::key_state() && KEY_DOWN)
	{
		velocity.z -= 0.1;
	}

	if (KeyInput::key_state() && KEY_RIGHT)
	{
		velocity.x += 0.1;
	}

	if (KeyInput::key_state() && KEY_LEFT)
	{
		velocity.x -= 0.1;
	}

	// DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	// DirectX::XMVECTOR Right = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&right));

	// 入力されたスピードのZ方向を X
	DirectX::XMFLOAT3 pos, speed;
	DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR sX =  DirectX::XMLoadFloat(&speed.x);
	DirectX::XMVECTOR sZ =  DirectX::XMLoadFloat(&speed.z);

	// DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Front, sZ));
	// DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Right, sX));

}
