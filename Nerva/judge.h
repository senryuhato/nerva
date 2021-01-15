#pragma once
#include <DirectXMath.h>

// �w�肵�����l�̊ԂɃN�����v����(�f�t�H���g��0 �` 1)
void clamp_(float& v, float min = 0, float max = 1);

// �_�ƒ����̍ŒZ����
// p:�_ sp:�����̎n�_ ep:�����̏I�_ h:�_����~�낵�������Ƃ̌�_(�߂�l) t:�x�N�g���W��(�߂�l) �߂�l:�ŒZ����
float PointLineDist(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 sp, DirectX::XMFLOAT3 ep, DirectX::XMFLOAT3& h, float t);

// 2�̒������s�p�ł��邩�ǂ���
// v1:����(�x�N�g��) v2:����(�x�N�g��)
bool isActuteAngle(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

// �_�Ɛ����̍ŒZ����
// p: �_ seg0:�����̎n�_ seg1: �����̏I�_ h: �ŒZ�����ƂȂ�[�_( �߂�l ) t: �[�_�̈ʒu( t<0:�n�_�̊O, 0<t<1:������, t>1:�I�_�̊O) �߂�l: �ŒZ����
float PointSegmentDist(const DirectX::XMFLOAT3 p, const DirectX::XMFLOAT3 seg0, const DirectX::XMFLOAT3 seg1, DirectX::XMFLOAT3& h, float t);

// 2�����̍ŒZ����
// p1:�����̎n�_ e1:�����̏I�_ p2:������̒����̎n�_ e2:������̒����̏I�_ h1:e1-p1����̐����Ƃ̌�_( �߂�l ) h2:e2-p2����̐����Ƃ̌�_( �߂�l ) t1:e1-p1�����̃x�N�g���W��( �߂�l ) t2:e2-p2�����̃x�N�g���W��( �߂�l ) �߂�l:�ŒZ����
float LineLineDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2);

// 2�����̍ŒZ����
// p1:����1�̎n�_ e1:����1�̏I�_p2:����2�̎n�_ e2:����2�̏I�_ h1:����1���̐����̌�_( �߂�l ) h2:����2���̐����̌�_( �߂�l ) t1:����1���̃x�N�g���W��( �߂�l ) t2:����2���̃x�N�g���W��( �߂�l ) �߂�l:�ŒZ����
float SegmentSegmentDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2);

// �J�v�Z�����m�̏Փ˔���
// p1:�J�v�Z��1�̉~�����̈�[ e1:�J�v�Z��1�̉~�����̂�����[ r1:�J�v�Z��1�̔��a p2:�J�v�Z��2�̉~�����̈�[ e2:�J�v�Z��2�̉~�����̂�����[ r2:�J�v�Z��2�̔��a
bool isHitCapsuleCapsule(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const float r1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, const float r2);
