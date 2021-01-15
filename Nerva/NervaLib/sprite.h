#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include "render_state.h"
#include "shader.h"

class SpriteShader : public Shader
{
public:
	SpriteShader(ID3D11Device* device)
	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		vs_name = "Shaders/sprite_vs.cso";
		ps_name = "Shaders/sprite_ps.cso";

		create_vs_from_cso(device, vs_name.c_str(), vertex_shader.GetAddressOf(),
			input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device, ps_name.c_str(), pixel_shader.GetAddressOf());
	}
};

class Sprite
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	struct CbSubset
	{
		DirectX::XMFLOAT4	material_color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> subset_constant_buffer;

	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	std::unique_ptr<DepthStencilState> depth_stencil_state;
	std::unique_ptr<SamplerState> sampler_state;
	std::unique_ptr<RasterizerState> rasterizer_state;

	std::shared_ptr<Shader> shader;

public:
	Sprite(ID3D11Device* device, const wchar_t* texture_file_name);
	virtual ~Sprite() {}
	void set_shader(std::shared_ptr<Shader>& shader) { this->shader = shader; }

	void render(ID3D11DeviceContext* immediate_context,
		DirectX::XMFLOAT2 position = {},
		DirectX::XMFLOAT2 scale = { 1.0f,1.0f },
		float angle = {},
		DirectX::XMFLOAT2 size = {},
		DirectX::XMFLOAT2 tex_pos = {},
		DirectX::XMFLOAT2 tex_size = {},
		DirectX::XMFLOAT2 center = {},
		DirectX::XMFLOAT4 material_color = { 1.0f,1.0f,1.0f,1.0f }
	);
};