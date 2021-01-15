#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <memory>

#include "render_state.h"

class FullscreenQuad
{
public:
	FullscreenQuad(ID3D11Device* device);
	virtual ~FullscreenQuad() = default;
	FullscreenQuad(FullscreenQuad&) = delete;
	FullscreenQuad& operator=(FullscreenQuad&) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> embedded_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shader;

	std::unique_ptr<RasterizerState> embedded_rasterizer_state;
	std::unique_ptr<DepthStencilState> embedded_depth_stencil_state;
	std::unique_ptr<SamplerState> embedded_sampler_state;

public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	void blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView* shader_resource_view,
		bool use_embedded_rasterizer_state = true, bool use_embedded_depth_stencil_state = true, bool use_embedded_pixel_shader = false);
};