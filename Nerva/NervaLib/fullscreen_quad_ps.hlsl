#include "fullscreen_quad.hlsli"

Texture2D texture_map : register(t0);
SamplerState texture_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	return texture_map.Sample(texture_map_sampler_state, pin.texcoord);
}