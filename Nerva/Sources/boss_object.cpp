#include "../NervaLib/key_input.h"
#include "boss_object.h"
#include "framework.h"

void BossObject::update(std::shared_ptr<Collision> collision)
{
	float elaspsed_time = Framework::instance().get_elapsed_time();

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

		/*int materialIndex = collision->ray_pick(startPosition, endPosition, &outPosition, &out_normal);
		{
			transform.position.x = outPosition.x;
			transform.position.z = outPosition.y;
			transform.position.z = outPosition.z;
		}*/
		//if (materialIndex != -1)
		//{
		//	/*transform.position.x = outPosition.x;
		//	transform.position.z = outPosition.z;*/

		//	{
		//		startPosition.x = transform.position.x;
		//		startPosition.y = transform.position.y + 1.0f;
		//		startPosition.z = transform.position.z;
		//		endPosition.x = startPosition.x + x;
		//		endPosition.y = startPosition.y;
		//		endPosition.z = startPosition.z + z;

		//		outPosition = endPosition;

		//		int materialIndex = collision->ray_pick(startPosition, endPosition, &outPosition, &out_normal);
		//		{
		//			transform.position.x = outPosition.x + out_normal.x;
		//			transform.position.z = outPosition.z + out_normal.z;
		//		}

		//	/*	int materialIndex = collision->move_check(startPosition, endPosition, &outPosition);
		//		transform.position.x = outPosition.x;
		//		transform.position.z = outPosition.z;*/
		//	}
		//}
		
		//int materialIndex = collision->move_check(startPosition, endPosition, &outPosition);
		//transform.position.x = outPosition.x;
		//transform.position.z = outPosition.z;
	}


	switch (move_type)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:

		break;
	case 3:
		
		break;
	case 4:

		break;
	}

	

	//地面当たり判定
	{
		//float gravity = 1.0f * elaspsed_time;

		//transform.position.y -= gravity;

		DirectX::XMFLOAT3 ray_start(transform.position.x, transform.position.y + 10.0f, transform.position.z);//レイを飛ばす開始位置
		DirectX::XMFLOAT3 ray_end(transform.position.x, transform.position.y - 10.0f, transform.position.z);//レイを飛ばす終了位置
		DirectX::XMFLOAT3 out_position;//レイの当たった位置
		DirectX::XMFLOAT3 out_normal;//レイの当たった面の法線

		if (-1 != collision->ray_pick(ray_start, ray_end, &out_position, &out_normal))
		{
			transform.position.y = out_position.y;
		}
	}

	transform.rotation = transform.euler(transform.angle);
	transform.update();


}

