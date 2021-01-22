#include "framework.h"
#include "../NervaLib/key_input.h"
#include "TPCamera.h"

void TPCamera::update()
{
	//POINT cursor;
	//::GetCursorPos(&cursor);

	//old_cursor = current_cursor;
	//current_cursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	//float move_x = (current_cursor.x - old_cursor.x) * 0.02f;
	//float move_y = (current_cursor.y - old_cursor.y) * 0.02f;

	//// if (KeyInput::key_state() & KEY_ALT)
	//{

	//	{
	//		// YŽ²‰ñ“]
	//		rotate.y += move_x * 0.5f;
	//		if (rotate.y > DirectX::XM_PI/2)
	//		{
	//			rotate.y -= DirectX::XM_2PI;
	//		}
	//		else if (rotate.y < -DirectX::XM_PI/2)
	//		{
	//			rotate.y += DirectX::XM_2PI;
	//		}
	//		// XŽ²‰ñ“]
	//		rotate.x += move_y * 0.5f;
	//		if (rotate.x > DirectX::XM_PI)
	//		{
	//			rotate.x -= DirectX::XM_2PI;
	//		}
	//		else if (rotate.x < -DirectX::XM_PI)
	//		{
	//			rotate.x += DirectX::XM_2PI;
	//		}
	//		
	//	}
	//}

	//RECT rc;
	//GetClientRect(Framework::instance().get_hwnd(), &rc);
	//float screen_width = rc.right - rc.left;
	//float screen_height = rc.bottom - rc.top;
	//if (current_cursor.x > screen_width / 2 + 1 || current_cursor.x < screen_width / 2 - 1 || current_cursor.y > screen_width / 2 + 1 || current_cursor.y < screen_height / 2 - 1)
	//{
	//	::SetCursorPos(screen_width / 2, screen_height / 2);
	//	::GetCursorPos(&cursor);
	//	current_cursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
	//	old_cursor = current_cursor;
	//}

	DirectX::XMMATRIX r_matrix = {};
	r_matrix = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	DirectX::XMVECTOR right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), r_matrix);
	DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), r_matrix);
	DirectX::XMVECTOR front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), r_matrix);

	DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&this->focus);
	DirectX::XMVECTOR distance = DirectX::XMVectorSet(this->distance, this->distance, this->distance, 0.0f);
	DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));

	DirectX::XMFLOAT3 minus = { -1,-1,-1 };

	DirectX::XMStoreFloat3(&this->eye, eye);
	DirectX::XMStoreFloat3(&this->up, up);
	DirectX::XMStoreFloat3(&this->right, right);
	DirectX::XMStoreFloat3(&this->front, front);
	// DirectX::XMStoreFloat3(&this->focus, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&minus), DirectX::XMVectorMultiply(front, distance)));

	set_view_matrix();

}
