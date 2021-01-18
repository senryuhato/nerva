#pragma once

#include "model_object.h"

class PlayerObject : public ModelObject
{
private:
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
protected:
	DirectX::XMFLOAT3 Cfront = { 0,0,1 };
	DirectX::XMFLOAT3 Cright = { 1,0,0 };

public:
	// �ړ��n
	// �L�[���͂ɂ��ړ�
	void move();
	// �J�����̐��ʂƉE�������Ƃ��Ă��邽�߂̊֐�
	void setCameraFrontRight(const DirectX::XMFLOAT3 f, const DirectX::XMFLOAT3 r);
	// 
	void direction();
	

	void update();
};
