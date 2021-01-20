#pragma once
#include <DirectXMath.h>

// 指定した数値の間にクランプする(デフォルトが0 ～ 1)
void clamp_(float& v, const  float min = 0, const  float max = 1);

// 点と直線の最短距離
// p:点 sp:直線の始点 ep:直線の終点 h:点から降ろした垂線との交点(戻り値) t:ベクトル係数(戻り値) 戻り値:最短距離
float PointLineDist(const DirectX::XMFLOAT3 p, const  DirectX::XMFLOAT3 sp, const  DirectX::XMFLOAT3 ep, DirectX::XMFLOAT3& h, float& t);

// 2つの直線の角度のθの値
// v1:直線(ベクトル) v2:直線(ベクトル)
float dotAngleRadians(const DirectX::XMFLOAT3 v1, const  DirectX::XMFLOAT3 v2);

// 2つの直線の角度のθの値
// v1:直線(ベクトル) v2:直線(ベクトル)
float CrossAngleRadians(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2);

// 2つの直線が鋭角であるかどうか
// v1:直線(ベクトル) v2:直線(ベクトル)
bool isActuteAngle(const DirectX::XMFLOAT3 v1, const  DirectX::XMFLOAT3 v2);

// 点と線分の最短距離
// p: 点 seg0:線分の始点 seg1: 線分の終点 h: 最短距離となる端点( 戻り値 ) t: 端点の位置( t<0:始点の外, 0<t<1:線分内, t>1:終点の外) 戻り値: 最短距離
float PointSegmentDist(const DirectX::XMFLOAT3 p, const DirectX::XMFLOAT3 seg0, const DirectX::XMFLOAT3 seg1, DirectX::XMFLOAT3& h, float& t);

// 2直線の最短距離
// p1:直線の始点 e1:直線の終点 p2:もう一つの直線の始点 e2:もう一つの直線の終点 h1:e1-p1線上の垂線との交点( 戻り値 ) h2:e2-p2線上の垂線との交点( 戻り値 ) t1:e1-p1線側のベクトル係数( 戻り値 ) t2:e2-p2線側のベクトル係数( 戻り値 ) 戻り値:最短距離
float LineLineDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2);

// 2線分の最短距離
// p1:線分1の始点 e1:線分1の終点p2:線分2の始点 e2:線分2の終点 h1:線分1側の垂線の交点( 戻り値 ) h2:線分2側の垂線の交点( 戻り値 ) t1:線分1側のベクトル係数( 戻り値 ) t2:線分2側のベクトル係数( 戻り値 ) 戻り値:最短距離
float SegmentSegmentDist(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, DirectX::XMFLOAT3& h1, DirectX::XMFLOAT3& h2, float& t1, float& t2);

// カプセル同士の衝突判定
// p1:カプセル1の円柱部の一端 e1:カプセル1の円柱部のもう一端 r1:カプセル1の半径 p2:カプセル2の円柱部の一端 e2:カプセル2の円柱部のもう一端 r2:カプセル2の半径
bool isHitCapsuleCapsule(const DirectX::XMFLOAT3 p1, const DirectX::XMFLOAT3 e1, const float r1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, const float r2);

// 球とカプセルの衝突判定
// p1:球の中心点 r1:球の半径 p2:カプセルの円柱部の一端 e2:カプセルの円柱部のもう一端 r2:カプセルの半径
bool isHitSphere(const DirectX::XMFLOAT3 p1, const float r1, const DirectX::XMFLOAT3 p2, const DirectX::XMFLOAT3 e2, const float r2);
