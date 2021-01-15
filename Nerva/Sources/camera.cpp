#include "camera.h"

#include "framework.h"

#include "../NervaLib/key_input.h"

void Camera::initialize()
{
	Framework& framework = Framework::instance();
	D3D11_VIEWPORT viewport = framework.get_viewport();

	fov_y = DirectX::XMConvertToRadians(30.0f);
	aspect = (float)viewport.Width / (float)viewport.Height;
	perspective_near_z = 0.1f;
	perspective_far_z = 10000.0f;
	set_perspective_projection(fov_y, aspect, perspective_near_z, perspective_far_z);

	eye = { 0.0f, 5.0f, -25.0f };
	focus = { 0.0f, 0.0f, 0.0f };
	up = { 0.0f,1.0f,0.0f };

	float x = focus.x - eye.x;
	float y = focus.y - eye.y;
	float z = focus.z - eye.z;

	distance = sqrtf(x * x + y * y + z * z);

	set_view_matrix();
}

void Camera::set_view_matrix()
{
	DirectX::XMVECTOR position, target, up_vec;
	position = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f);
	target = DirectX::XMVectorSet(focus.x, focus.y, focus.z, 1.0f);
	up_vec = DirectX::XMVectorSet(up.x, up.y, up.z, 1.0f);

	DirectX::XMMATRIX v = {};
	v = DirectX::XMMatrixLookAtLH(position, target, up_vec);
	DirectX::XMStoreFloat4x4(&view, v);
}

void Camera::set_perspective_projection(float fov_y, float aspect, float perspective_near_z, float perspective_far_z)
{
	this->fov_y = fov_y;
	this->aspect = aspect;
	this->perspective_near_z = perspective_near_z;
	this->perspective_far_z = perspective_far_z;

	DirectX::XMMATRIX p = {};
	p = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, perspective_near_z, perspective_far_z);

	DirectX::XMStoreFloat4x4(&projection, p);
}

void Camera::update()
{
	POINT cursor;
	::GetCursorPos(&cursor);

	old_cursor = current_cursor;
	current_cursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	float move_x = (current_cursor.x - old_cursor.x) * 0.02f;
	float move_y = (current_cursor.y - old_cursor.y) * 0.02f;

	if (KeyInput::key_state() & KEY_ALT)
	{
		if (KeyInput::key_state() & KEY_LBUTTON)
		{
			// YŽ²‰ñ“]
			rotate.y += move_x * 0.5f;
			if (rotate.y > DirectX::XM_PI)
			{
				rotate.y -= DirectX::XM_2PI;
			}
			else if (rotate.y < -DirectX::XM_PI)
			{
				rotate.y += DirectX::XM_2PI;
			}
			// XŽ²‰ñ“]
			rotate.x += move_y * 0.5f;
			if (rotate.x > DirectX::XM_PI)
			{
				rotate.x -= DirectX::XM_2PI;
			}
			else if (rotate.x < -DirectX::XM_PI)
			{
				rotate.x += DirectX::XM_2PI;
			}
		}
		else if (KeyInput::key_state() & KEY_MBUTTON)
		{
			// •½sˆÚ“®
			float s = distance * 0.035f;
			float x = -move_x * s;
			float y = move_y * s;

			focus.x += right.x * x;
			focus.y += right.y * x;
			focus.z += right.z * x;

			focus.x += up.x * y;
			focus.y += up.y * y;
			focus.z += up.z * y;
		}
		else if (KeyInput::key_state() & KEY_RBUTTON)
		{
			// ƒY[ƒ€
			focus.x += front.x * (-move_y - move_x) * 2.0f;
			focus.y += front.y * (-move_y - move_x) * 2.0f;
			focus.z += front.z * (-move_y - move_x) * 2.0f;
		}
	}

	DirectX::XMMATRIX r_matrix = {};
	r_matrix = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	DirectX::XMVECTOR right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), r_matrix);
	DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), r_matrix);
	DirectX::XMVECTOR front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), r_matrix);

	DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&this->focus);
	DirectX::XMVECTOR distance = DirectX::XMVectorSet(this->distance, this->distance, this->distance, 0.0f);
	DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));

	DirectX::XMStoreFloat3(&this->eye, eye);
	DirectX::XMStoreFloat3(&this->up, up);
	DirectX::XMStoreFloat3(&this->right, right);
	DirectX::XMStoreFloat3(&this->front, front);

	set_view_matrix();
}