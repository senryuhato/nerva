#pragma once

#include "static_object.h"

class Collision
{
public: 
	std::shared_ptr<StaticObject> terrain;

	// 地形の登録と解除 
	void register_terrain(std::shared_ptr<StaticObject>& obj);
	void unregister_terrain();
	// 登録されている地形にたいしてのレイピック 
	int ray_pick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float range = 0.0f);
	// 登録されている地形に対しての反射  
	int reflect(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, float rate, DirectX::XMFLOAT3* out_position, DirectX::XMFLOAT3* out_reflect, float range = 0.0f);

	int move_check(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, DirectX::XMFLOAT3* out_position, float range = 0.0f);
	int move_check02(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, DirectX::XMFLOAT3* out_position, float range = 0.0f);
};
