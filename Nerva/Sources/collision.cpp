#include "collision.h"

#include "player_object.h"
#include "boss_object.h"
#include "judge.h"

void Collision::register_terrain(std::shared_ptr<StaticObject>& obj)
{
	terrain = obj;
}

void Collision::unregister_terrain()
{
	if (terrain)
		terrain.reset();
}


int Collision::ray_pick(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, DirectX::XMFLOAT3* out_position, DirectX::XMFLOAT3* out_normal, float range)
{
	if (!terrain) return -1;
	return terrain->ray_pick(start_position, end_position, out_position, out_normal, range);
}

int Collision::reflect(const DirectX::XMFLOAT3& start_position, const DirectX::XMFLOAT3& end_position, float rate, DirectX::XMFLOAT3* out_position, DirectX::XMFLOAT3* out_reflect, float range)
{
	if (!terrain) return -1;
	// レイピック  
	DirectX::XMFLOAT3 hitPosition, hitNormal;
	int materialIndex = ray_pick(start_position, end_position, &hitPosition, &hitNormal, range);
	if (materialIndex == -1) { return materialIndex; }

	// 壁までのベクトル 
	DirectX::XMVECTOR hit = DirectX::XMLoadFloat3(&hitPosition);
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&start_position);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&end_position);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);

	// 壁の法線  
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// 入射ベクトルを法線に射影  
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(DirectX::XMVectorScale(vec, -1), normal);

	// 反射位置の計算  
	// out = vec + normal * (dot * 2); 
	DirectX::XMVECTOR reflect = DirectX::XMVectorSubtract(DirectX::XMVectorAdd(end, DirectX::XMVectorMultiply(normal, DirectX::XMVectorScale(dot, 2))), start);

	// 反射率の補正  
	reflect = DirectX::XMVectorScale(reflect, 1.0f);
	// 反射位置(hit予定位置) 
	DirectX::XMVECTOR position = hit;
	DirectX::XMStoreFloat3(out_position, position);
	DirectX::XMStoreFloat3(out_reflect, reflect);

	return materialIndex;
}

int Collision::move_check(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, float range)
{
	if (!terrain) return -1;
	// レイピック 
	DirectX::XMFLOAT3 hitPosition, hitNormal;
	int materialIndex = ray_pick(startPosition, endPosition, &hitPosition, &hitNormal);
	if (materialIndex == -1)
	{
		// ヒットしなかったら移動後の位置は終点 
		*outPosition = endPosition;
		return materialIndex;
	}
	// 壁をつきぬけたベクトル 
	//{
	//	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&hitPosition);
	//	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	//	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(start, end);

	//	// 壁の法線ベクトルを単位化 
	//	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	//	// 入射ベクトルを法線に射影 
	//	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(normal, vec);

	//	// 補正位置の計算 
	//	DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, DirectX::XMVectorMultiply(normal, dot));

	//	DirectX::XMStoreFloat3(outPosition, position);
	//	//DirectX::XMStoreFloat3(outPosition, DirectX::XMLoadFloat3(&hitPosition));
	//}


	{
		DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
		DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);

		// 壁の法線
		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

		// 入射ベクトルを法線に射影
		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(vec), normal);


		DirectX::XMVECTOR position = DirectX::XMVectorMultiplyAdd(normal, dot, end);
		DirectX::XMStoreFloat3(outPosition, position);
	}




	// 補正後の位置が壁にめり込んでいた場合は移動しないようにする 
	if (ray_pick(startPosition, *outPosition, &hitPosition, &hitNormal) != -1)
	{
		*outPosition = startPosition;
		return materialIndex;
	}

	return materialIndex;
}

int Collision::move_check02(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, float range)
{
	if (!terrain) return -1;
	// レイピック 
	DirectX::XMFLOAT3 hitPosition, hitNormal;
	int materialIndex = ray_pick(startPosition, endPosition, &hitPosition, &hitNormal);
	if (materialIndex == -1)
	{
		// ヒットしなかったら移動後の位置は終点 
		*outPosition = endPosition;
		return materialIndex;
	}

	// 壁をつきぬけたベクトル 
	{
		DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&hitPosition);
		DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(start, end);

		// 壁の法線ベクトルを単位化 
		hitNormal.y = 0.0f;
		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

		// 入射ベクトルを法線に射影 
		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(normal, vec);

		// 補正位置の計算 
		DirectX::XMVECTOR position = DirectX::XMVectorAdd(end, DirectX::XMVectorMultiply(normal, dot));

		DirectX::XMStoreFloat3(outPosition, position);
	}

	// 補正後の位置が壁にめり込んでいた場合は移動しないようにする 
	if (ray_pick(startPosition, *outPosition, &hitPosition, &hitNormal) != -1)
	{
		*outPosition = startPosition;
		return materialIndex;
	}

	return materialIndex;
}

#include <string>

void Hit::hit_judge(std::shared_ptr<PlayerObject> pl, std::shared_ptr<BossObject> bs)
{
	DirectX::XMFLOAT3 p_root_1 = {};
	DirectX::XMFLOAT3 p_root_2 = {};
	for (auto& it : pl->model->nodes)
	{
		std::string str1 = "joint1";
		std::string str2 = "joint2";
		if (it.name == str1)
		{
			p_root_1 =
			{
				it.world_transform._41,
				it.world_transform._42,
				it.world_transform._43,
			};
		}
		if(it.name == str2)
		{
			p_root_2 =
			{
				it.world_transform._41,
				it.world_transform._42,
				it.world_transform._43,
			};
		}
	}

	DirectX::XMFLOAT3 bs_head = {};
	DirectX::XMFLOAT3 bs_teil1 = {};
	DirectX::XMFLOAT3 bs_teil2 = {};
	for (auto& it : bs->model->nodes)
	{
		std::string str1 = "atama";
		std::string str2 = "sippo1";
		std::string str3 = "sippo7";
		if (it.name == str1)
		{
			bs_head =
			{
				it.world_transform._41,
				it.world_transform._42,
				it.world_transform._43,
			};
		}
		if (it.name == str2)
		{
			bs_teil1 =
			{
				it.world_transform._41,
				it.world_transform._42,
				it.world_transform._43,
			};
		}
		if (it.name == str3)
		{
			bs_teil2 =
			{
				it.world_transform._41,
				it.world_transform._42,
				it.world_transform._43,
			};
		}
	}

	if (pl->hit)
	{
		if (isHitCapsuleCapsule(p_root_1, p_root_2, 1.0f, bs_head, bs_teil2, 8))
		{
			pl->hit = false;
			bs->hp -= 5;
		}
	}

	if (bs->hit)
	{
		if (isHitSphereCapsule(bs_head, 8, pl->transform.position, DirectX::XMFLOAT3(pl->transform.position.x, pl->transform.position.y+8, pl->transform.position.z),1))
		{
			bs->hit = false;
			pl->hp -= 5;
		}
	}
}