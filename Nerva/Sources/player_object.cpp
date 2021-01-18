#include "../NervaLib/key_input.h"
#include "judge.h"
#include "player_object.h"

void PlayerObject::update()
{
	move();
	direction();

	transform.update();
	transform.calculate_world_transform_quaternion();
}

// setFrontRightされること前提になっている
void PlayerObject::move()
{
	DirectX::XMFLOAT3 s = {2.0f,2.0f,2.0f};
	DirectX::XMVECTOR sX = DirectX::XMLoadFloat3(&s);
	DirectX::XMVECTOR sZ = DirectX::XMLoadFloat3(&s);
	DirectX::XMVECTOR vX = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMVECTOR vZ = DirectX::XMLoadFloat3(&velocity);
	if (KeyInput::key_state() & KEY_UP)
	{
		vZ = DirectX::XMVectorAdd(vZ, sZ);
	}

	if (KeyInput::key_state() & KEY_DOWN)
	{
		vZ = DirectX::XMVectorSubtract(vZ, sZ);
	}

	if (KeyInput::key_state() & KEY_RIGHT)
	{
		vX = DirectX::XMVectorAdd(vX, sX);
	}

	if (KeyInput::key_state() & KEY_LEFT)
	{
		vX = DirectX::XMVectorSubtract(vX, sX);
	}


	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Cfront));
	DirectX::XMVECTOR Right = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Cright));

	// 入力されたスピードのZ方向を X
	DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&transform.position);

	Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Front, vZ));
	Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Right, vX));

	DirectX::XMStoreFloat3(&velocity, DirectX::XMVectorAdd(vX, vZ));
	velocity.x /= 10, velocity.z /= 10;
	// DirectX::XMStoreFloat(&velocity.x, vX);

	DirectX::XMStoreFloat3(&transform.position, Pos);
	transform.position.y = 0;
}

void PlayerObject::setCameraFrontRight(const DirectX::XMFLOAT3 f, const DirectX::XMFLOAT3 r)
{
	Cfront = f;
	Cright = r;
}

void PlayerObject::direction()
{
	DirectX::XMVECTOR f = DirectX::XMLoadFloat3(&Cfront);
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&Cright);

	DirectX::XMFLOAT3 v1;
	DirectX::XMStoreFloat3(&v1, DirectX::XMVector3Normalize(DirectX::XMVectorAdd(f, r)));

	transform.angle.y = angleRadians(velocity, v1);
	// こっちでできるが velocity が0の時,姿が消える
	// transform.rotation.y = angleRadians(velocity, v1);
}
