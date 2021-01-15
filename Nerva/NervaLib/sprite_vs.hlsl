#include "lambert.hlsli"

VS_OUT main(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
	vout.position = position;
	vout.color.xyz = material_color.xyz;
	vout.color.w = material_color.w;

	vout.texcoord = texcoord;

	return vout;
}