#include "judge.h"

void clamp_(float& v, float min, float max)
{
	if (v < min)
	{
		v = min;
	}
	else if (v > max)
	{
		v = max;
	}
}

float PointLineDist(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 sp, DirectX::XMFLOAT3 ep, DirectX::XMFLOAT3& h, float t)
{// �������v�Z�̑���VECTOR�֓����
	using namespace DirectX;
	XMVECTOR P, Sp, Ep;
	P = XMLoadFloat3(&p);
	Sp = XMLoadFloat3(&sp);
	Ep = XMLoadFloat3(&ep);

	// �����̃x�N�g��
	XMVECTOR V = XMVectorSubtract(Ep, Sp);
	// �x�N�g���̒���
	XMVECTOR D;
	D = XMVector3Length(V);

	t = 0;
	float v;
	XMStoreFloat(&v, D);
	// �����ƒ��������݂��邩
	if (v > 0.0f)
	{
		// t = V�E(P - Sp) / V�EV
		XMStoreFloat(&t, XMVectorDivide(XMVector3Dot(V, XMVectorSubtract(P, Sp)), XMVector3Dot(V, V)));
	}
	// �_����~�낵�������������̂ǂ̈ʒu�ɑ��݂��邩
	XMVECTOR T = XMLoadFloat(&t);
	// h = Sp + T * V
	XMStoreFloat3(&h, XMVectorAdd(Sp, XMVectorMultiply(T, V)));

	float d;
	// d=vec3Length(h-p)
	XMStoreFloat(&d, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&h), P)));
	return d;
}

bool isActuteAngle(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	using namespace DirectX;

	XMVECTOR V1, V2, L1, L2;
	V1 = XMLoadFloat3(&v1);
	V2 = XMLoadFloat3(&v2);

	L1 = XMVector3Length(V1);
	L2 = XMVector3Length(V2);

	// ���ςƃx�N�g���̒������g����cos�Ƃ����߂�
	// Dot = vec3Dot(v1,v2) / (l1*l2);
	XMVECTOR Dot = XMVectorDivide(XMVector3Dot(V1, V2), XMVectorMultiply(L1, L2));

	// cos�Ƃ���Ƃ����߂�
	float sita;
	XMStoreFloat(&sita, XMVectorACos(Dot));
	// sita���s�p(0�x���傫��90�x��菬����)�Ȃ�
	if (XMConvertToRadians(sita) < 90 && XMConvertToRadians(sita) < 0)
	{// true
		return true;
	}// �Ⴄ�Ȃ�false
	return false;

}

float PointSegmentDist(const DirectX::XMFLOAT3 p, const DirectX::XMFLOAT3 seg0, const DirectX::XMFLOAT3 seg1, DirectX::XMFLOAT3& h, float t)
{
	// �����̒���(d),�����Ɛ����̌�_�̍��W(h)����уx�N�g���W��(t)���Z�o
	float d = PointLineDist(p, seg0, seg1, h, t);
	using namespace DirectX;
	XMVECTOR P, Seg0, Seg1;
	// �v�Z�̂��߂�XMVECTOR�ɂ���
	P = XMLoadFloat3(&p);
	Seg0 = XMLoadFloat3(&seg0);
	Seg1 = XMLoadFloat3(&seg1);
	// �p�x���s�p�����ׂ邽�߂Ƀx�N�g�������
	XMFLOAT3 p_seg0,p_seg1, seg1_0,seg0_1;
	XMStoreFloat3(&p_seg0, XMVectorSubtract(P, Seg0));// p - seg0
	XMStoreFloat3(&p_seg1, XMVectorSubtract(P, Seg1));// p - seg1
	XMStoreFloat3(&seg0_1, XMVectorSubtract(Seg0, Seg1));// seg0 - 1
	XMStoreFloat3(&seg1_0, XMVectorSubtract(Seg1, Seg0));// seg1 - 0

	if (isActuteAngle(p_seg0, seg1_0) == false)
	{// �n�_���̊O��
		h = seg0;
		XMVECTOR L = XMVector3Length(XMVectorSubtract(Seg0, P));
		float l;
		XMStoreFloat(&l, L);
		return l;
	}
	else if (isActuteAngle(p_seg1, seg0_1) == false)
	{// �I�_���̊O��
		h = seg1;
		XMVECTOR L = XMVector3Length(XMVectorSubtract(Seg1, P));
		float l;
		XMStoreFloat(&l, L);
		return l;
	}

	return d;
}

float LineLineDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2)
{
	using namespace DirectX;

	XMVECTOR P1, E1, P2, E2;
	P1 = XMLoadFloat3(&p1);
	E1 = XMLoadFloat3(&e1);
	P2 = XMLoadFloat3(&p2);
	E2 = XMLoadFloat3(&e2);
	XMVECTOR V1 = XMVectorSubtract(E1, P1);
	XMVECTOR V2 = XMVectorSubtract(E2, P2);
	// 2���������s��
	XMVECTOR Cross = XMVector3Cross(V1, V2);
	float l;
	XMStoreFloat(&l, XMVector3Length(Cross));
	if (l == 0)
	{// ���s�Ȃ璼����̂ǂ̈ʒu�ɐ��������낵�Ă��ŒZ�����ɂȂ�̂�
		// �_�ƒ����̍ŒZ���������߂�
		float d = PointLineDist(p1, p2, e2, h2, t2);
		h1 = p1;
		t1 = 0.0f;

		return d;
	}

	/* 2�����͂˂���̊֌W*/
	
	// �x�N�g���W�������߂�
	// t1 = D3DXVec3Dot(&v1, &v2) * D3DXVec3Dot(&v2, &(p1 - p2)) - D3DXVec3Dot(&v2, &v2) * D3DXVec3Dot(&v1, &(p1 - p2));
	// t1 /= D3DXVec3Dot(&v1, &v1) * D3DXVec3Dot(&v2, &v2) - D3DXVec3Dot(&v1, &v2) * D3DXVec3Dot(&v1, &v2);
	XMStoreFloat(&t1, XMVectorDivide(XMVectorSubtract(XMVectorMultiply(XMVector3Dot(V1, V2), XMVector3Dot(V2, XMVectorSubtract(P1, P2))), XMVectorMultiply(XMVector3Dot(V2, V2), XMVector3Dot(V1, XMVectorSubtract(P1, P2)))), XMVectorMultiply(XMVector3Dot(V1, V1), XMVectorSubtract(XMVector3Dot(V2, V2), XMVectorMultiply(XMVector3Dot(V1, V2), XMVector3Dot(V1, V2))))));
	XMVECTOR T1 = XMLoadFloat(&t1);
	
	// ��_�̍��W�����߂�
	// h1 = p1 + v1 * t1;
	XMStoreFloat3(&h1, XMVectorAdd(P1, XMVectorMultiply(V1, T1)));
	XMVECTOR H1 = XMLoadFloat3(&h1);
	
	// �x�N�g���W�������߂�
	XMStoreFloat(&t2, XMVectorDivide(XMVector3Dot(V2, XMVectorSubtract(H1, P2)), XMVector3Dot(V2, V2)));
	XMVECTOR T2 = XMLoadFloat(&t2);
	
	// ��_�̍��W�����߂�
	XMStoreFloat3(&h2, XMVectorAdd(P2, XMVectorMultiply(V2, T2)));
	XMVECTOR H2 = XMLoadFloat3(&h2);

	// �ŒZ���������߂�
	float h2_1;
	XMStoreFloat(&h2_1, XMVectorSubtract(H2, H1));
	return h2_1;
}

float SegmentSegmentDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2)
{
	using namespace DirectX;
	// �v�Z�̂��߂�XMVECTOR�ɓ����
	XMVECTOR P1, E1, P2, E2;
	P1 = XMLoadFloat3(&p1);
	E1 = XMLoadFloat3(&e1);
	P2 = XMLoadFloat3(&p2);
	E2 = XMLoadFloat3(&e2);

	XMVECTOR V1 = XMVectorSubtract(E1, P1);
	XMVECTOR V2 = XMVectorSubtract(E2, P2);
	XMVECTOR L1 = XMVector3Length(V1);
	XMVECTOR L2 = XMVector3Length(V2);
	float l1, l2;
	XMStoreFloat(&l1, L1);
	XMStoreFloat(&l2, L2);

	float epsilon = 0.0001;
	// ����1���k�ނ��Ă��邩?
	if (l1 < epsilon)
	{// ����2���k�ނ��Ă��邩?
		if (l2 < epsilon)
		{// �����Ƃ��k�ނ��Ă��邽�ߗ������̎n�_�Ǝx�X�̋��������߂�
			float d;
			XMStoreFloat(&d, XMVector3Length(XMVectorSubtract(P2, P1)));
			h1 = p1;
			h2 = p2;
			t1 = t2 = 0.0f;
			return d;
		}
		else
		{// ����1�������k�ނ��Ă��邽�ߐ���1�̎n�_�Ɛ���2�̍ŒZ���������߂�
			float d = PointSegmentDist(p1, p2, e2, h2, t2);
			h1 = p1;
			t1 = 0.0f;
			clamp_(t2, 0, 1);
			return d;
		}
	}// ����2���k�ނ��Ă��邩?
	else if (l2 < epsilon)
	{// ����2�̎n�_�Ɛ���1�̍ŒZ���������߂�
		float d = PointSegmentDist(p2, p1, e1, h1, t1);
		h2 = p2;
		t2 = 0.0f;
		clamp_(t1, 0, 1);
		return d;
	}

	/* �������m�̍ŒZ���� */
	float l_cross;
	XMStoreFloat(&l_cross, XMVector3Length(XMVector3Cross(V1, V2)));
	// 2���������s�������琂���̒[�_��1�����̌�_�Ƃ���
	if (l_cross < epsilon)
	{
		t1 = 0.0f;
		h1 = p1;
		float d = PointSegmentDist(p1, p2, e2, h2, t2);
		// �����̊O�֏o�Ă��邩�m�F
		if (0.0f <= t2 && t2 <= 1.0f)
		{// �o�ĂȂ�
			return d;
		}
	}
	else
	{// �����͂˂���̊֌W
		// 2�����̍ŒZ���������߂ĉ���t1,t2�����߂�
		float d = LineLineDist(p1, e1, p2, e2, h1, h2, t1, t2);
		// �����̊O�֏o�Ă��邩�m�F
		if (0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f)
		{// �o�ĂȂ�
			return d;
		}
	}

	// �����̊O�Ɍ�_������ꍇ�����܂Ői��
	// ����1��t1��0�`1�̊ԂɃN�����v���Đ���2�Ƃ̌�_�����߂�
	clamp_(t1, 0, 1);
	XMVECTOR T1 = XMLoadFloat(&t1);
	XMStoreFloat3(&h1, XMVectorAdd(P1, XMVectorMultiply(T1, V1)));
	float d = PointSegmentDist(h1, p2, e2, h2, t2);
	// ����2�̊O�ɏo�Ă��邩�m�F
	if (0.0f <= t2 && t2 <= 1.0f)
	{// �o�ĂȂ�
		return d;
	}

	// ����2�����O�������̂Ő���2���N�����v���Đ���1�Ƃ̌�_�����߂�
	clamp_(t2, 0, 1);
	XMVECTOR T2 = XMLoadFloat(&t2);
	XMStoreFloat3(&h2, XMVectorAdd(P2, XMVectorMultiply(T2, V2)));
	d = PointSegmentDist(h2, p1, e1, h1, t1);
	// ����1�̊O�ɏo�Ă��邩�m�F
	if (0.0f <= t1 && t1 <= 1.0f)
	{// �o�ĂȂ�
		return d;
	}

	// �������̒[�_�̋������ŒZ�Ɣ���
	clamp_(t1, 0, 1);
	T1 = XMLoadFloat(&t1);
	XMStoreFloat3(&h1, XMVectorAdd(P1, XMVectorMultiply(T1, V1)));

	XMVECTOR H1 = XMLoadFloat3(&h1);
	XMVECTOR H2 = XMLoadFloat3(&h1);

	float l;
	XMStoreFloat(&l,XMVector3Length(XMVectorSubtract(H2, H1)));
	return l;
}

bool isHitCapsuleCapsule(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const float r1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, const float r2)
{
	using namespace DirectX;
	// �O���Ɏ����Ă��������ꍇ�͈����ɒǉ����Ă�������
	XMFLOAT3 h1, h2;
	float t1, t2;
	float d = SegmentSegmentDist(p1, e1, p2, e2, h1, h2, t1, t2);
	return (d < r1 + r2);
}
