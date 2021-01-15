#pragma once

#include <wrl.h>
#include <d3d11.h>

class BlendState
{
public:
	enum BLEND_MODE
	{
		NONE,
		ALPHA,
		ADD,
		SUBTRACT,
		REPLACE,
		MULTIPLY,
		LIGHTEN,
		DARKEN,
		SCREEN,
		ALPHA_TO_COVERAGE,
		END
	};

private:
	ID3D11BlendState* create_blend_state(ID3D11Device* device, BLEND_MODE mode);

public:
	Microsoft::WRL::ComPtr<ID3D11BlendState> state_object;

	BlendState(ID3D11Device* device, BLEND_MODE mode);
	BlendState(ID3D11Device* device, const D3D11_BLEND_DESC* blend_desc);
	void activate(ID3D11DeviceContext* immediate_context);
	void deactivate(ID3D11DeviceContext* immediate_context);

public:
	~BlendState() = default;
	BlendState(BlendState&) = delete;
	BlendState& operator =(BlendState&) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> cached_state_object;
};

class RasterizerState
{
public:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> state_object;

	RasterizerState(ID3D11Device* device, D3D11_FILL_MODE fill_mode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE cull_mode = D3D11_CULL_BACK, bool front_counter_clockwise = true,
		bool antialiasing = true, bool depth_clip_enable = true);
	RasterizerState(ID3D11Device* device, const D3D11_RASTERIZER_DESC* rasterizer_desc);
	void activate(ID3D11DeviceContext* immediate_context);
	void deactivate(ID3D11DeviceContext* immediate_context);

public:
	~RasterizerState() = default;
	RasterizerState(RasterizerState&) = delete;
	RasterizerState& operator =(RasterizerState&) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cached_state_object;
};

class DepthStencilState
{
public:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state_object;

	DepthStencilState(ID3D11Device* device, BOOL depth_enable = TRUE,
		D3D11_DEPTH_WRITE_MASK depth_write_mask = D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_FUNC depth_func = D3D11_COMPARISON_LESS);
	DepthStencilState(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC* depth_stencil_desc);
	void activate(ID3D11DeviceContext* immediate_context);
	void deactivate(ID3D11DeviceContext* immediate_context);

public:
	~DepthStencilState() = default;
	DepthStencilState(DepthStencilState&) = delete;
	DepthStencilState& operator =(DepthStencilState&) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_state_object;
};

class SamplerState
{
public:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> state_object = nullptr;

	SamplerState(ID3D11Device* device, D3D11_SAMPLER_DESC sampler_desc);
	SamplerState(ID3D11Device* device, D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER,
		float r = 1, float g = 1, float b = 1, float a = 1);
	void activate(ID3D11DeviceContext* immediate_context, UINT slot, bool set_in_vs = false);
	void deactivate(ID3D11DeviceContext* immediate_context);

public:
	~SamplerState() = default;
	SamplerState(SamplerState&) = delete;
	SamplerState& operator =(SamplerState&) = delete;

private:
	UINT using_slot = 0;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> cached_state_objects[2];
};