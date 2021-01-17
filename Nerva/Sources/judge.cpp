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

float PointLineDist(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 sp, DirectX::XMFLOAT3 ep, DirectX::XMFLOAT3& h, float& t)
{// 引数を計算の速いVECTORへ入れる
	using namespace DirectX;
	XMVECTOR P, Sp, Ep;
	P = XMLoadFloat3(&p);
	Sp = XMLoadFloat3(&sp);
	Ep = XMLoadFloat3(&ep);

	// 直線のベクトル
	XMVECTOR V = XMVectorSubtract(Ep, Sp);
	// ベクトルの長さ
	XMVECTOR D;
	D = XMVector3Length(V);

	t = 0;
	float v;
	XMStoreFloat(&v, D);
	// ちゃんと長さが存在するか
	if (v > 0.0f)
	{
		// t = V・(P - Sp) / V・V
		XMStoreFloat(&t, XMVectorDivide(XMVector3Dot(V, XMVectorSubtract(P, Sp)), XMVector3Dot(V, V)));
	}
	// 点から降ろした垂線が直線のどの位置に存在するか
	XMVECTOR T = XMLoadFloat(&t);
	// h = Sp + T * V
	XMStoreFloat3(&h, XMVectorAdd(Sp, XMVectorMultiply(T, V)));

	float d;
	// d=vec3Length(h-p)
	XMStoreFloat(&d, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&h), P)));
	return d;
}

float angleRadians(const DirectX::XMFLOAT3 v1, const  DirectX::XMFLOAT3 v2)
{
	using namespace DirectX;

	XMVECTOR V1, V2, L1, L2;
	V1 = XMLoadFloat3(&v1);
	V2 = XMLoadFloat3(&v2);

	L1 = XMVector3Length(V1);
	L2 = XMVector3Length(V2);

	// 内積とベクトルの長さを使ってcosθを求める
	// Dot = vec3Dot(v1,v2) / (l1*l2);
	XMVECTOR Dot = XMVectorDivide(XMVector3Dot(V1, V2), XMVectorMultiply(L1, L2));

	// cosθからθを求める
	float sita;
	XMStoreFloat(&sita, XMVectorACos(Dot));

	return XMConvertToRadians(sita);
}

bool isActuteAngle(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	using namespace DirectX;

	// sitaが鋭角(0度より大きく90度より小さい)なら
	if (angleRadians(v1, v2) < 90 && angleRadians(v1, v2) < 0)
	{// true
		return true;
	}// 違うならfalse
	return false;

}

float PointSegmentDist(const DirectX::XMFLOAT3 p, const DirectX::XMFLOAT3 seg0, const DirectX::XMFLOAT3 seg1, DirectX::XMFLOAT3& h, float& t)
{
	// 垂線の長さ(d),直線と垂線の交点の座標(h)およびベクトル係数(t)を算出
	float d = PointLineDist(p, seg0, seg1, h, t);
	using namespace DirectX;
	XMVECTOR P, Seg0, Seg1;
	// 計算のためにXMVECTORにする
	P = XMLoadFloat3(&p);
	Seg0 = XMLoadFloat3(&seg0);
	Seg1 = XMLoadFloat3(&seg1);
	// 角度が鋭角か調べるためにベクトルを作る
	XMFLOAT3 p_seg0,p_seg1, seg1_0,seg0_1;
	XMStoreFloat3(&p_seg0, XMVectorSubtract(P, Seg0));// p - seg0
	XMStoreFloat3(&p_seg1, XMVectorSubtract(P, Seg1));// p - seg1
	XMStoreFloat3(&seg0_1, XMVectorSubtract(Seg0, Seg1));// seg0 - 1
	XMStoreFloat3(&seg1_0, XMVectorSubtract(Seg1, Seg0));// seg1 - 0

	if (isActuteAngle(p_seg0, seg1_0) == false)
	{// 始点側の外側
		h = seg0;
		XMVECTOR L = XMVector3Length(XMVectorSubtract(Seg0, P));
		float l;
		XMStoreFloat(&l, L);
		return l;
	}
	else if (isActuteAngle(p_seg1, seg0_1) == false)
	{// 終点側の外側
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
	// 2直線が平行か
	XMVECTOR Cross = XMVector3Cross(V1, V2);
	float l;
	XMStoreFloat(&l, XMVector3Length(Cross));
	if (l == 0)
	{// 平行なら直線状のどの位置に垂線を下ろしても最短距離になるので
		// 点と直線の最短距離を求める
		float d = PointLineDist(p1, p2, e2, h2, t2);
		h1 = p1;
		t1 = 0.0f;

		return d;
	}

	/* 2直線はねじれの関係*/
	
	// ベクトル係数を求める
	// t1 = D3DXVec3Dot(&v1, &v2) * D3DXVec3Dot(&v2, &(p1 - p2)) - D3DXVec3Dot(&v2, &v2) * D3DXVec3Dot(&v1, &(p1 - p2));
	// t1 /= D3DXVec3Dot(&v1, &v1) * D3DXVec3Dot(&v2, &v2) - D3DXVec3Dot(&v1, &v2) * D3DXVec3Dot(&v1, &v2);
	XMStoreFloat(&t1, XMVectorDivide(XMVectorSubtract(XMVectorMultiply(XMVector3Dot(V1, V2), XMVector3Dot(V2, XMVectorSubtract(P1, P2))), XMVectorMultiply(XMVector3Dot(V2, V2), XMVector3Dot(V1, XMVectorSubtract(P1, P2)))), XMVectorMultiply(XMVector3Dot(V1, V1), XMVectorSubtract(XMVector3Dot(V2, V2), XMVectorMultiply(XMVector3Dot(V1, V2), XMVector3Dot(V1, V2))))));
	XMVECTOR T1 = XMLoadFloat(&t1);
	
	// 交点の座標を求める
	// h1 = p1 + v1 * t1;
	XMStoreFloat3(&h1, XMVectorAdd(P1, XMVectorMultiply(V1, T1)));
	XMVECTOR H1 = XMLoadFloat3(&h1);
	
	// ベクトル係数を求める
	XMStoreFloat(&t2, XMVectorDivide(XMVector3Dot(V2, XMVectorSubtract(H1, P2)), XMVector3Dot(V2, V2)));
	XMVECTOR T2 = XMLoadFloat(&t2);
	
	// 交点の座標を求める
	XMStoreFloat3(&h2, XMVectorAdd(P2, XMVectorMultiply(V2, T2)));
	XMVECTOR H2 = XMLoadFloat3(&h2);

	// 最短距離を求める
	float h2_1;
	XMStoreFloat(&h2_1, XMVectorSubtract(H2, H1));
	return h2_1;
}

float SegmentSegmentDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2)
{
	using namespace DirectX;
	// 計算のためにXMVECTORに入れる
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
	// 線分1が縮退しているか?
	if (l1 < epsilon)
	{// 線分2が縮退しているか?
		if (l2 < epsilon)
		{// 両方とも縮退しているため両線分の始点と支店の距離を求める
			float d;
			XMStoreFloat(&d, XMVector3Length(XMVectorSubtract(P2, P1)));
			h1 = p1;
			h2 = p2;
			t1 = t2 = 0.0f;
			return d;
		}
		else
		{// 線分1だけが縮退しているため線分1の始点と線分2の最短距離を求める
			float d = PointSegmentDist(p1, p2, e2, h2, t2);
			h1 = p1;
			t1 = 0.0f;
			clamp_(t2, 0, 1);
			return d;
		}
	}// 線分2が縮退しているか?
	else if (l2 < epsilon)
	{// 線分2の始点と線分1の最短距離を求める
		float d = PointSegmentDist(p2, p1, e1, h1, t1);
		h2 = p2;
		t2 = 0.0f;
		clamp_(t1, 0, 1);
		return d;
	}

	/* 線分同士の最短距離 */
	float l_cross;
	XMStoreFloat(&l_cross, XMVector3Length(XMVector3Cross(V1, V2)));
	// 2線分が平行だったら垂線の端点の1つを仮の交点とする
	if (l_cross < epsilon)
	{
		t1 = 0.0f;
		h1 = p1;
		float d = PointSegmentDist(p1, p2, e2, h2, t2);
		// 線分の外へ出ているか確認
		if (0.0f <= t2 && t2 <= 1.0f)
		{// 出てない
			return d;
		}
	}
	else
	{// 線分はねじれの関係
		// 2直線の最短距離を求めて仮のt1,t2を求める
		float d = LineLineDist(p1, e1, p2, e2, h1, h2, t1, t2);
		// 線分の外へ出ているか確認
		if (0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f)
		{// 出てない
			return d;
		}
	}

	// 線分の外に交点がある場合ここまで進む
	// 線分1のt1を0～1の間にクランプして線分2との交点を求める
	clamp_(t1, 0, 1);
	XMVECTOR T1 = XMLoadFloat(&t1);
	XMStoreFloat3(&h1, XMVectorAdd(P1, XMVectorMultiply(T1, V1)));
	float d = PointSegmentDist(h1, p2, e2, h2, t2);
	// 線分2の外に出ているか確認
	if (0.0f <= t2 && t2 <= 1.0f)
	{// 出てない
		return d;
	}

	// 線分2側が外だったので線分2をクランプして線分1との交点を求める
	clamp_(t2, 0, 1);
	XMVECTOR T2 = XMLoadFloat(&t2);
	XMStoreFloat3(&h2, XMVectorAdd(P2, XMVectorMultiply(T2, V2)));
	d = PointSegmentDist(h2, p1, e1, h1, t1);
	// 線分1の外に出ているか確認
	if (0.0f <= t1 && t1 <= 1.0f)
	{// 出てない
		return d;
	}

	// 両線分の端点の距離が最短と判明
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
	// 外部に持っていきたい場合は引数に追加してください
	XMFLOAT3 h1, h2;
	float t1, t2;
	float d = SegmentSegmentDist(p1, e1, p2, e2, h1, h2, t1, t2);
	return (d < r1 + r2);
}

bool isHitSphere(const DirectX::XMFLOAT3 p1, const float r1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, const float r2)
{
	using namespace DirectX;
	XMFLOAT3 h1, h2;
	float t1, t2;
	float d = PointSegmentDist(p1, p2, e2, h2, t2);
	return (d < r1 + r2);
}

//TODO:2021 これを自機の移動のところに入れれば行けると思います
void playerControl(DirectX::XMFLOAT3 front,DirectX::XMFLOAT3 right)
{// カメラの正面と右方向が分かればいい
	using namespace DirectX;
	XMVECTOR Front = XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR Right = XMVector3Normalize(XMLoadFloat3(&right));
	
	// 入力されたスピードのZ方向を X
	XMFLOAT3 pos, speed;
	XMVECTOR Pos = XMLoadFloat3(&pos);
	XMVECTOR sX = XMLoadFloat(&speed.x);
	XMVECTOR sZ = XMLoadFloat(&speed.z);

	XMVectorAdd(Pos, XMVectorMultiply(Front, sZ));
	XMVectorAdd(Pos, XMVectorMultiply(Right, sX));

}
