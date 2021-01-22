#include "../NervaLib/key_input.h"
#include "boss_object.h"
#include "framework.h"

void BossObject::update(std::shared_ptr<Collision> collision, std::shared_ptr<ModelObject> model_object)
{
	float elaspsed_time = Framework::instance().get_elapsed_time();

	int motion_type = 0;

	switch (motion_type)
	{
	case 0:
	{
		DirectX::XMFLOAT3 dis_pos = 
		{
			model_object->transform.position.x - transform.position.x,
			model_object->transform.position.y - transform.position.y,
			model_object->transform.position.z - transform.position.z
		};

		DirectX::XMVECTOR pos_1 = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dis_pos));

		DirectX::XMFLOAT3 this_pos = DirectX::XMFLOAT3(transform.world._41, transform.world._42, transform.world._43);

		DirectX::XMVECTOR pos_2 = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&this_pos));

		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(pos_1, pos_2);

		float a_cos;
		DirectX::XMStoreFloat(&a_cos, dot);
		if (a_cos < -1.0f) a_cos = -1.0f;
		if (a_cos > 1.0f) a_cos = 1.0f;
		a_cos = acosf(a_cos);

		transform.angle.y += a_cos;
	}
		break;
	}


	if (KeyInput::key_state() & KEY_RIGHT)
	{
		transform.angle.y += DirectX::XMConvertToRadians(45) * elaspsed_time;
	}
	if (KeyInput::key_state() & KEY_LEFT)
	{
		transform.angle.y -= DirectX::XMConvertToRadians(45) * elaspsed_time;
	}

	//壁当たり判定
	if (KeyInput::key_state() & KEY_UP)
	{
		const float speed = 60.0f * elaspsed_time;

		float x = sinf(transform.angle.y) * speed;
		float z = cosf(transform.angle.y) * speed;

		DirectX::XMFLOAT3 startPosition = {};
		DirectX::XMFLOAT3 endPosition = {};
		DirectX::XMFLOAT3 outPosition = {};
		DirectX::XMFLOAT3 out_normal = {};

		startPosition.x = transform.position.x;
		startPosition.y = transform.position.y + 1.0f;
		startPosition.z = transform.position.z;
		endPosition.x = startPosition.x + x;
		endPosition.y = startPosition.y;
		endPosition.z = startPosition.z + z;
		
		/*outPosition = endPosition;*/

		collision->move_check(startPosition, endPosition, &outPosition);
		transform.position.x = outPosition.x;
		transform.position.y = outPosition.y - 1.0f;
		transform.position.z = outPosition.z;

	}

	//地面当たり判定
	//{
	//	DirectX::XMFLOAT3 ray_start(transform.position.x, transform.position.y + 10.0f, transform.position.z);//レイを飛ばす開始位置
	//	DirectX::XMFLOAT3 ray_end(transform.position.x, transform.position.y - 10.0f, transform.position.z);//レイを飛ばす終了位置
	//	DirectX::XMFLOAT3 out_position;//レイの当たった位置
	//	DirectX::XMFLOAT3 out_normal;//レイの当たった面の法線

	//	if (-1 != collision->ray_pick(ray_start, ray_end, &out_position, &out_normal))
	//	{
	//		transform.position.y = out_position.y;
	//	}
	//}

	transform.rotation = transform.euler(transform.angle);
	transform.update();
}

