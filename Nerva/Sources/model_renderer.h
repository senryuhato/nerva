#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "../NervaLib/model.h"
#include "../NervaLib/shader.h"
#include "../NervaLib/render_state.h"

class LambertShader : public Shader
{
public:
	LambertShader(ID3D11Device* device)
	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		vs_name = "Shaders/lambert_vs.cso";
		ps_name = "Shaders/lambert_ps.cso";

		create_vs_from_cso(device, vs_name.c_str(), vertex_shader.GetAddressOf(),
			input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device, ps_name.c_str(), pixel_shader.GetAddressOf());
	}
};

class ModelRenderer
{
public:
	static const int max_bones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4	view_projection;
		DirectX::XMFLOAT4	light_direction;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	bone_transforms[max_bones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	material_color;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mesh_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> subset_constant_buffer;

	std::unique_ptr<RasterizerState> rasterizer_state;
	std::unique_ptr<DepthStencilState> depth_stencil_state;
	std::unique_ptr<SamplerState> sampler_state;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	dummy_shader_resource_view;

public:
	std::shared_ptr<Shader> shader;

	void set_shader(std::shared_ptr<Shader>& shader) { this->shader = shader; }
	void begin(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction);
	void draw(ID3D11DeviceContext* context, const Model* model);
	void end(ID3D11DeviceContext* context);

	ModelRenderer(ID3D11Device* device, bool cull_mode = false);
};