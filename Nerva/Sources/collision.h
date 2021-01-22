#pragma once

#include "static_object.h"

class Collision
{
public: 
	std::shared_ptr<StaticObject> terrain;

	// �n�`�̓o�^�Ɖ��� 
	void register_terrain(std::shared_ptr<StaticObject>& obj);
	void unregister_terrain();
	// �o�^����Ă���n�`�ɂ������Ẵ��C�s�b�N 
	int ray_pick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float range = 0.0f);
	// �o�^����Ă���n�`�ɑ΂��Ă̔���  
	int reflect(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, float rate, DirectX::XMFLOAT3* out_position, DirectX::XMFLOAT3* out_reflect, float range = 0.0f);

	int move_check(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, DirectX::XMFLOAT3* out_position, float range = 0.0f);
	int move_check02(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, DirectX::XMFLOAT3* out_position, float range = 0.0f);
};
