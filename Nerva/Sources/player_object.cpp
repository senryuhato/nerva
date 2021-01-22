#include "../NervaLib/key_input.h"
#include "framework.h"
#include "judge.h"
#include "player_object.h"

void PlayerObject::update()
{
	move();
	direction();
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
void PlayerObject::move()
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

	if (KeyInput::key_trigger() & KEY_RBUTTON)
	{
		float vxL, vzL;
		DirectX::XMStoreFloat(&vxL, DirectX::XMVector3Length(vX));
		DirectX::XMStoreFloat(&vzL, DirectX::XMVector3Length(vZ));
		// キー入力されていないなら向いてる方向に回避する
		if (vxL == 0 && vzL == 0)
		{

			vX = DirectX::XMVectorAdd(vX, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&front), sX));
			vZ = DirectX::XMVectorAdd(vZ, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&front), sZ));
		}

		float a = 10;
		DirectX::XMFLOAT3 avoid = { a,0,a };
		DirectX::XMVECTOR Avoid = DirectX::XMLoadFloat3(&avoid);
		vX = DirectX::XMVectorMultiply(vX, Avoid);
		vZ = DirectX::XMVectorMultiply(vZ, Avoid);
		// アニメーションを回避に切り替える
	}

	DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&axisFront));
	DirectX::XMVECTOR Right = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&axisRight));

	// 入力されたスピードのZ方向を X
	DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&transform.position);

	Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Front, vZ));
	Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorMultiply(Right, vX));

	DirectX::XMStoreFloat3(&velocity, DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Right, vX), DirectX::XMVectorMultiply(Front, vZ)));

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

	DirectX::XMStoreFloat3(&transform.position, Pos);
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
		}
	}
	


	if (!this->model->is_play_animation())
	{
		if (pursuit1)
		{
			this->model->play_animation(2, false);
			pursuit1 = false;
		}
		else if(pursuit2)
		{
			this->model->play_animation(3, false);
			pursuit2 = false;
		}
		else
		{
			this->model->play_animation(5, true);
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

void PlayerObject::direction()
{
	return;
	POINT cursor;
	::GetCursorPos(&cursor);

	old_cursor = current_cursor;
	current_cursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	float move_x = (current_cursor.x - old_cursor.x) * 0.02f;
	float move_y = (current_cursor.y - old_cursor.y) * 0.02f;

	// if (KeyInput::key_state() & KEY_ALT)
	//{

	//	{
	//		// Y軸回転
	//		rotate.y += move_x * 0.5f;
	//		if (rotate.y > DirectX::XM_PI / 2)
	//		{
	//			rotate.y -= DirectX::XM_2PI;
	//		}
	//		else if (rotate.y < -DirectX::XM_PI / 2)
	//		{
	//			rotate.y += DirectX::XM_2PI;
	//		}
	//		if (rotate.y > DirectX::XMConvertToRadians(60))rotate.y = DirectX::XMConvertToRadians(60);
	//		else if (rotate.y < DirectX::XMConvertToRadians(-60))rotate.y = DirectX::XMConvertToRadians(-60);

	//		// X軸回転
	//		rotate.x += move_y * 0.5f;
	//		if (rotate.x > DirectX::XM_PI/2)
	//		{
	//			rotate.x -= DirectX::XM_2PI;
	//		}
	//		else if (rotate.x < -DirectX::XM_PI/2)
	//		{
	//			rotate.x += DirectX::XM_2PI;
	//		}
	//		if (rotate.x > DirectX::XMConvertToRadians(60))rotate.x = DirectX::XMConvertToRadians(60);
	//		else if (rotate.x < DirectX::XMConvertToRadians(-60))rotate.x = DirectX::XMConvertToRadians(-60);

	//	}
	//}

	RECT rc;
	GetClientRect(Framework::instance().get_hwnd(), &rc);
	float screen_width = rc.right - rc.left;
	float screen_height = rc.bottom - rc.top;
	if (current_cursor.x > screen_width / 2 + 1 || current_cursor.x < screen_width / 2 - 1 || current_cursor.y > screen_width / 2 + 1 || current_cursor.y < screen_height / 2 - 1)
	{
		::SetCursorPos(screen_width / 2, screen_height / 2);
		::GetCursorPos(&cursor);
		current_cursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
		old_cursor = current_cursor;
	}

	DirectX::XMMATRIX r_matrix = {};
	r_matrix = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	DirectX::XMVECTOR right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), r_matrix);
	DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), r_matrix);
	DirectX::XMVECTOR front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), r_matrix);

	
	DirectX::XMStoreFloat3(&this->axisUp, up);
	DirectX::XMStoreFloat3(&this->axisRight, right);
	DirectX::XMStoreFloat3(&this->axisFront, front);

	
	/*DirectX::XMVECTOR f = DirectX::XMLoadFloat3(&Cfront);
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&Cright);

	DirectX::XMFLOAT3 v1;
	DirectX::XMStoreFloat3(&v1, DirectX::XMVectorAdd(f, r));*/

	// transform.angle.y += dotAngleRadians(front, velocity);
	transform.angle.y += dotAngleRadians(velocity, this->front);

	// DirectX::XMFLOAT4 axis = { 0.0f,1.0f,0.0f,1.0f };
	// DirectX::XMFLOAT4 rotate = { 0.0f,0.0f,0.0f,1.0f };
	// float tanjent;
	// DirectX::XMFLOAT3 tanjent;
	// transform.angle.y = angleRadians(velocity, v1);
	// DirectX::XMStoreFloat3(&tanjent, DirectX::XMVectorATan2(DirectX::XMLoadFloat3(&velocity),DirectX::XMLoadFloat3(&v1)));
	// DirectX::XMVECTOR delta = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat4(&axis), tanjent);
	// angleRadians(v1,velocity)

	// DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&rotate), delta));

}
