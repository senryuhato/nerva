#pragma once

#include "model_object.h"
#include "collision.h"

class PlayerObject : public ModelObject
{
private:
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
protected:
	DirectX::XMFLOAT3 front = { 0,0,1 };
	DirectX::XMFLOAT3 Cfront = { 0,0,1 };
	DirectX::XMFLOAT3 Cright = { 1,0,0 };
	DirectX::XMFLOAT3 Cangle = { 0,0,0 };

public:
	DirectX::XMFLOAT2 current_cursor = {};
	DirectX::XMFLOAT2 old_cursor = {};

	DirectX::XMFLOAT3 rotate = {};

	DirectX::XMFLOAT3 axisRight = { 1,0,0 };
	DirectX::XMFLOAT3 axisUp = { 0,1,0 };
	DirectX::XMFLOAT3 axisFront = { 0,0,1 };

	float hp = 100;

public:
	// �ړ��n
	// �L�[���͂ɂ��ړ�
	void move(std::shared_ptr<Collision> collision);
	void attack();
	
	// �J�����̐��ʂƉE�������Ƃ��Ă��邽�߂̊֐�
	void setCameraFrontRight(const DirectX::XMFLOAT3 f, const DirectX::XMFLOAT3 r);
	void setCameraAngle(const DirectX::XMFLOAT3 rotate);
	// 
	void direction();
	// �ǂƂ̓����蔻����Ƃ�֐�
	void hitWall(std::shared_ptr<Collision> collision);
	
	// void initialize();
	void update(std::shared_ptr<Collision> collision);
};
