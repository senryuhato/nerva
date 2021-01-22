#include "../NervaLib/key_input.h"
#include "boss_object.h"
#include "framework.h"
#include "imgui.h"

void BossObject::update(std::shared_ptr<Collision> collision, std::shared_ptr<ModelObject> model_object)
{
	float elaspsed_time = Framework::instance().get_elapsed_time();

	if (hp > 0)
	{

		DirectX::XMFLOAT3 orient(transform.world._31, transform.world._32, transform.world._33);
		DirectX::XMFLOAT3 p_orient =
		{
		model_object->transform.position.x - transform.position.x,
		model_object->transform.position.y - transform.position.y,
		model_object->transform.position.z - transform.position.z,
		};


		DirectX::XMVECTOR orient_ = DirectX::XMLoadFloat3(&orient);
		DirectX::XMVECTOR p_orient_ = DirectX::XMLoadFloat3(&p_orient);

		float len;
		DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(p_orient_));

		orient_ = DirectX::XMVector3Normalize(orient_);
		p_orient_ = DirectX::XMVector3Normalize(p_orient_);

		float dot;

		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(orient_, p_orient_));
		float angle = acosf(dot);

		switch (motion_type)
		{
		case 0:
		{
			switch (move_type)
			{
			case 0:
				//todo : 00
				model->play_animation(0, true);
				move_type++;
				break;
			case 1:
			{

				if (len > 30)
				{
					if (dot > 0.90)
					{
						motion_type = 2;
						move_type = 0;
					}
					else
					{
						motion_type = 1;
						move_type = 0;
					}

				}
				/*else if (len > 16)
				{
					motion_type = 1;
					move_type = 0;
				}*/
				else if (len < 30)
				{
					if (dot > 0.90)
					{
						motion_type = 3;
						move_type = 0;
					}
					else
					{
						motion_type = 1;
						move_type = 0;
					}
				}
			}
			break;
			}
		}
		break;
		case 1:
		{
			switch (move_type)
			{
			case 0:
				//todo : 00
				model->play_animation(4, true);
				move_type++;
				break;
			case 1:
			{
				if (len == 0.0f) break;

				if (dot >= 0.99f)
				{
					motion_type = 0;
					move_type = 0;
				}

				DirectX::XMVECTOR cross = DirectX::XMVector3Cross(orient_, p_orient_);
				DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(cross));
				if (len == 0.0f)
				{
					DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&transform.rotation), DirectX::XMLoadFloat4(&transform.euler({ 0.0f, DirectX::XMConvertToRadians(45) * elaspsed_time, 0.0f }))));
					break;
				}
				DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(cross, angle);

				DirectX::XMVECTOR orien = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&transform.rotation), quaternion);

				DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&transform.rotation), orien, 0.01f));
			}
			break;
			}
		}
		break;
		case 2:
		{
			switch (move_type)
			{
			case 0:
				//todo : 00
				model->play_animation(2, true);
				timer = 0.0f;
				move_type++;
				break;
			case 1:
			{


				transform.position =
				{
					transform.position.x + orient.x * 2.0f,
					transform.position.y + orient.y * 2.0f,
					transform.position.z + orient.z * 2.0f
				};

				if (timer > 0.5f)
				{
					motion_type = 0;
					move_type = 0;
				}
				timer += elaspsed_time;
			}
			break;
			}
		}
		break;
		case 3:
		{
			switch (move_type)
			{
			case 0:
				//todo : 00
				model->play_animation(1, false);
				timer = 0.0f;
				move_type++;
				break;
			case 1:
			{
				if (!model->is_play_animation())
				{
					motion_type = 0;
					move_type = 0;
				}
			}
			break;
			}
		}
		break;
		}


		/*if (KeyInput::key_state() & KEY_RIGHT)
		{
			DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&transform.rotation), DirectX::XMLoadFloat4(&transform.euler({ 0.0f, DirectX::XMConvertToRadians(45) * elaspsed_time, 0.0f }))));
		}
		if (KeyInput::key_state() & KEY_LEFT)
		{
			DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&transform.rotation), DirectX::XMLoadFloat4(&transform.euler({ 0.0f, DirectX::XMConvertToRadians(-45) * elaspsed_time, 0.0f }))));
		}*/

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

		//transform.rotation = transform.euler(transform.angle);
		transform.update();
	}
}

