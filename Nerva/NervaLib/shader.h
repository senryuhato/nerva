#pragma once
#include <d3d11.h>

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <string>

class Shader
{
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	//’Ç‰Á‚Ìsampler_state, shader_resource_viwe

	std::string vs_name;
	std::string ps_name;

public:
	Shader() {}
	Shader(ID3D11Device* device)
	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		vs_name = "Shaders/fullscreen_quad_vs.cso";
		ps_name = "Shaders/fullscreen_quad_ps.cso";

		create_vs_from_cso(device, vs_name.c_str(), vertex_shader.GetAddressOf(),
			input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device, ps_name.c_str(), pixel_shader.GetAddressOf());
	}

	virtual void activate(ID3D11DeviceContext* context)
	{
		context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		context->PSSetShader(pixel_shader.Get(), nullptr, 0);
		context->IASetInputLayout(input_layout.Get());
	}
	virtual void deactivate(ID3D11DeviceContext* context)
	{
		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
	}
	virtual ~Shader() {}
};