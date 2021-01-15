#pragma once

#include "../NervaLib/render_state.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>


class Sprite
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	material_color;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> subset_constant_buffer;

	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	std::unique_ptr<DepthStencilState> depth_stencil_state;
	std::unique_ptr<SamplerState> sampler_state;
	std::unique_ptr<RasterizerState> rasterizer_state;

public:
	Sprite(ID3D11Device* device, const wchar_t* texture_file_name);
	virtual ~Sprite() {}

	void render(ID3D11DeviceContext* immediate_context) {};

};