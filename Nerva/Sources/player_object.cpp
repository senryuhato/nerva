#include "../NervaLib/key_input.h"
#include "framework.h"
#include "judge.h"
#include "player_object.h"
#include "imgui.h"

void PlayerObject::update(std::shared_ptr<Collision> collision)
{
	move(collision);
	//	direction();
	attack();
	// transform.angle.y += 0.01;
// 	transform.rotation.y += 0.01;
	transform.rotation = transform.euler(transform.angle);

	DirectX::XMMATRIX r_matrix = DirectX::XMMatrixRotationRollPitchYaw(transform.angle.x, transform.angle.y, transform.angle.z);
	DirectX::XMVECTOR front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), r_matrix);
	DirectX::XMStoreFloat3(&this->front, front);

	transform.update();
}

// setFrontRightされること前提になっている
void PlayerObject::move(std::shared_ptr<Collision> collision)
{
	DirectX::XMFLOAT3 s = { 1.0f,0.0f,1.0f };
	DirectX::XMVECTOR sX = DirectX::XMLoadFloat3(&s);
	DirectX::XMVECTOR sZ = DirectX::XMLoadFloat3(&s);
	DirectX::XMVECTOR vX = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMVECTOR vZ = DirectX::XMLoadFloat3(&velocity);
	if (KeyInput::key_state() & KEY_W)
	{
		vZ = DirectX::XMVectorAdd(vZ, sZ);
		// transform.angle.y = 0;
	}

	if (KeyInput::key_state() & KEY_S)
	{
		vZ = DirectX::XMVectorSubtract(vZ, sZ);
		// transform.angle.y = DirectX::XMConvertToRadians(180);
	}

	if (KeyInput::key_state() & KEY_D)
	{
		vX = DirectX::XMVectorAdd(vX, sX);
		// transform.angle.y = DirectX::XMConvertToRadians(90);
	}

	if (KeyInput::key_state() & KEY_A)
	{
		vX = DirectX::XMVectorSubtract(vX, sX);
		// transform.angle.y = DirectX::XMConvertToRadians(-90);
	}

	//if (KeyInput::key_trigger() & KEY_RBUTTON)
	//{
	//	float vxL, vzL;
	//	DirectX::XMStoreFloat(&vxL, DirectX::XMVector3Length(vX));
	//	DirectX::XMStoreFloat(&vzL, DirectX::XMVector3Length(vZ));
	//	// キー入力されていないなら向いてる方向に回避する
	//	if (vxL == 0 && vzL == 0)
	//	{

	//		vX = DirectX::XMVectorAdd(vX, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&front), sX));
	//		vZ = DirectX::XMVectorAdd(vZ, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&front), sZ));
	//	}

	//	float a = 10;
	//	DirectX::XMFLOAT3 avoid = { a,0,a };
	//	DirectX::XMVECTOR Avoid = DirectX::XMLoadFloat3(&avoid);
	//	vX = DirectX::XMVectorMultiply(vX, Avoid);
	//	vZ = DirectX::XMVectorMultiply(vZ, Avoid);
	//	// アニメーションを回避に切り替える
	//}

	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Cfront));
	DirectX::XMVECTOR Right = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Cright));

	// 入力されたスピードのZ方向を X
	DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&transform.position);

	Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Front, vZ));
	Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Right, vX));

	DirectX::XMStoreFloat3(&velocity, DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Right, vX), DirectX::XMVectorMultiply(Front, vZ)));
	DirectX::XMStoreFloat3(&transform.position, Pos);

	hitWall(collision);
	float vxL, vzL;
	DirectX::XMStoreFloat(&vxL, DirectX::XMVector3Length(vX));
	DirectX::XMStoreFloat(&vzL, DirectX::XMVector3Length(vZ));
	if (vxL > 0 || vzL > 0)
	{
		if (model->current_animation != 0)
		{
			model->play_animation(0, true);
		}

		direction();
	}
	else
	{
		if (model->current_animation == 0)
		{
			model->play_animation(5, true);
		}
	}

	velocity.x /= 10, velocity.y = 0, velocity.z /= 10;
	if (velocity.x < 0.1)velocity.x = 0;
	if (velocity.z < 0.1)velocity.z = 0;
	// DirectX::XMStoreFloat(&velocity.x, vX);

	
	transform.position.y = 0;
}

void PlayerObject::attack()
{
	// 回避状態ならアタックできない
	// if(model->current_animation==?)return;
	static bool pursuit1 = false, pursuit2 = false;
	if (model->current_animation == 2)
	{
		if (KeyInput::key_trigger() & KEY_LBUTTON)
		{
			// アタック3を再生する
			// 同上
			pursuit2 = true;
		}
	}

	if (model->current_animation == 1)
	{
		if (KeyInput::key_trigger() & KEY_LBUTTON)
		{
			// アニメーションを次に進める
			// static bool で再生が終わったら続けて再生されるようにするとか
			pursuit1 = true;
		}
	}

	// 攻撃してない状態なら
	if (model->current_animation == 5 || model->current_animation < 1)
	{
		if (KeyInput::key_trigger() & KEY_LBUTTON)
		{
			// アニメーションをattack1回目にする
			this->model->play_animation(1, false);
			pursuit1 = false;
			pursuit2 = false;
			hit = false;
		}
	}



	if (!this->model->is_play_animation())
	{
		if (pursuit1)
		{
			this->model->play_animation(2, false);
			pursuit1 = false;
			hit = true;
		}
		else if (pursuit2)
		{
			this->model->play_animation(3, false);
			pursuit2 = false;
			hit = true;
		}
		else
		{
			this->model->play_animation(5, true);
			hit = false;
		}
	}


}


void PlayerObject::setCameraFrontRight(const DirectX::XMFLOAT3 f, const DirectX::XMFLOAT3 r)
{
	Cfront = f;
	Cright = r;
}

void PlayerObject::setCameraAngle(const DirectX::XMFLOAT3 rotate)
{
	Cangle = rotate;
}

void PlayerObject::hitWall(std::shared_ptr<Collision> collision)
{
	// float elaspsed_time = Framework::instance().get_elapsed_time();

	// const float speed = 60.0f * elaspsed_time;

	// float x = sinf(transform.angle.y) * speed;
	// float z = cosf(transform.angle.y) * speed;

	DirectX::XMFLOAT3 startPosition = {};
	DirectX::XMFLOAT3 endPosition = {};
	DirectX::XMFLOAT3 outPosition = {};
	DirectX::XMFLOAT3 out_normal = {};
	DirectX::XMFLOAT3 l;
	DirectX::XMStoreFloat3(&l, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&velocity)));
	startPosition.x = transform.position.x-velocity.x;
	startPosition.y = transform.position.y-velocity.y + 1;
	startPosition.z = transform.position.z-velocity.z;
	endPosition.x = transform.position.x;
	endPosition.y = transform.position.y;
	endPosition.z = transform.position.z;

	/*outPosition = endPosition;*/
	/*ImGui::Begin("rayCast");
	ImGui::Text("%f", velocity.x);
	ImGui::Text("%f", startPosition.z);
	ImGui::Text("%f", endPosition.z);
	ImGui::End();*/
	collision->move_check(startPosition, endPosition, &outPosition);
	transform.position.x = outPosition.x;
	transform.position.y = outPosition.y - 1.0f;
	transform.position.z = outPosition.z;


}

void PlayerObject::direction()
{

	transform.angle.y += dotAngleRadians(velocity, this->front);

}
