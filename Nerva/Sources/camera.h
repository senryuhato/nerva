#pragma once

#include <DirectXMath.h>
#include <string>

class Camera
{
protected:
	// projection
	float fov_y = 0;
	float aspect = 0;
	float perspective_near_z = .0f;
	float perspective_far_z = 0.0f;
	DirectX::XMFLOAT4X4 projection = {};

	// view
	DirectX::XMFLOAT3 eye = { .0f, .0f, 0.0f };
	DirectX::XMFLOAT3 focus = { .0f, .0f, .0f };
	DirectX::XMFLOAT3 up = { .0f, .0f, .0f };

	DirectX::XMFLOAT4X4 view = {};

	float distance = 0;

	DirectX::XMFLOAT2 old_cursor = {};
	DirectX::XMFLOAT2 current_cursor = {};
	DirectX::XMFLOAT3 rotate = {};
	float amount_x = 0;
	float amount_y = 0;

	DirectX::XMFLOAT3 right = { 1, 0, 0 };
	DirectX::XMFLOAT3 front = { 0, 0, 1 };
public:
	virtual ~Camera() = default;
	virtual void initialize();
	virtual void update();

public:
	void set_perspective_projection(float fov_y, float aspect, float perspective_near_z, float perspective_far_z);
	void set_view_matrix();
	float get_distance() { return distance; }
	const DirectX::XMFLOAT3& get_eye() { return eye; }
	const DirectX::XMFLOAT3& get_focus() { return focus; }
	const DirectX::XMFLOAT3& get_up() { return up; }
	void set_eye(const DirectX::XMFLOAT3& eye) { this->eye = eye; }
	void set_focus(const DirectX::XMFLOAT3& focus) { this->focus = focus; }
	void set_up(const DirectX::XMFLOAT3& up) { this->up = up; }
	const DirectX::XMFLOAT4X4& get_view() { return view; }
	const DirectX::XMFLOAT4X4& get_projection() { return projection; }
	const DirectX::XMFLOAT3& get_front() { return front; }
	const DirectX::XMFLOAT3& get_right() { return right; }
	const DirectX::XMFLOAT3& get_rotate() { return rotate; }
};
