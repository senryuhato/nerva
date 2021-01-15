#pragma once

#include <DirectXMath.h>

class Component
{
public:
	bool activate = true;
	Component() {}
	virtual ~Component() {}
};

class Transform : public Component
{
public:
	DirectX::XMFLOAT3 position = { 0 ,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4X4 world = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4 local_world = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4 parent_world = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
public:
	void initialize();
	void update();
	void calculate_world_transform_quaternion();
	const DirectX::XMFLOAT4 euler(const DirectX::XMFLOAT3& angle);
};
