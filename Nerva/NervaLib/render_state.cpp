#include "render_state.h"
#include "misc.h"

#include <d3d11.h>

ID3D11BlendState* BlendState::create_blend_state(ID3D11Device* device, BLEND_MODE mode)
{
	ID3D11BlendState* blend_state;

	D3D11_BLEND_DESC blend_desc = {};
	switch (mode)
	{
	case BLEND_MODE::NONE:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = FALSE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::ALPHA:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::ADD:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE D3D11_BLEND_SRC_ALPHA
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::SUBTRACT:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ZERO
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; //D3D11_BLEND_INV_SRC_COLOR
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT; //D3D11_BLEND_OP_ADD
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::REPLACE:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::MULTIPLY:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::LIGHTEN:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::DARKEN:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::SCREEN:
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3DBLEND_INVDESTCOLOR
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3DBLEND_ONE
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE::ALPHA_TO_COVERAGE:
		blend_desc.AlphaToCoverageEnable = TRUE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = FALSE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}
	HRESULT hr = device->CreateBlendState(&blend_desc, &blend_state);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return blend_state;
}

BlendState::BlendState(ID3D11Device* device, BLEND_MODE mode)
{
	state_object.Attach(create_blend_state(device, mode));
}

BlendState::BlendState(ID3D11Device* device, const D3D11_BLEND_DESC* blend_desc)
{
	HRESULT hr = device->CreateBlendState(blend_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void BlendState::activate(ID3D11DeviceContext* immediate_context)
{
	UINT sample_mask = 0xFFFFFFFF;
	immediate_context->OMGetBlendState(cached_state_object.ReleaseAndGetAddressOf(), 0, &sample_mask);
	immediate_context->OMSetBlendState(state_object.Get(), 0, 0xFFFFFFFF);
}

void BlendState::deactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->OMSetBlendState(cached_state_object.Get(), 0, 0xFFFFFFFF);
}

RasterizerState::RasterizerState(ID3D11Device* device, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode, bool front_counter_clockwise, bool antialiasing, bool depth_clip_enable)
{
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = fill_mode;
	rasterizer_desc.CullMode = cull_mode;
	rasterizer_desc.FrontCounterClockwise = front_counter_clockwise;
	rasterizer_desc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	rasterizer_desc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer_desc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer_desc.DepthClipEnable = static_cast<BOOL>(depth_clip_enable);
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = antialiasing && fill_mode == D3D11_FILL_SOLID ? TRUE : FALSE;
	rasterizer_desc.AntialiasedLineEnable = antialiasing ? TRUE : FALSE;
	HRESULT hr = device->CreateRasterizerState(&rasterizer_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

RasterizerState::RasterizerState(ID3D11Device* device, const D3D11_RASTERIZER_DESC* rasterizer_desc)
{
	HRESULT hr = device->CreateRasterizerState(rasterizer_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void RasterizerState::activate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->RSGetState(cached_state_object.ReleaseAndGetAddressOf());
	immediate_context->RSSetState(state_object.Get());
}

void RasterizerState::deactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->RSSetState(cached_state_object.Get());
}

DepthStencilState::DepthStencilState(ID3D11Device* device, BOOL depth_enable, D3D11_DEPTH_WRITE_MASK depth_write_mask, D3D11_COMPARISON_FUNC depth_func)
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = depth_enable;
	depth_stencil_desc.DepthWriteMask = depth_write_mask;
	depth_stencil_desc.DepthFunc = depth_func;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HRESULT hr = device->CreateDepthStencilState(&depth_stencil_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

DepthStencilState::DepthStencilState(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC* depth_stencil_desc)
{
	HRESULT hr = device->CreateDepthStencilState(depth_stencil_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void DepthStencilState::activate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->OMGetDepthStencilState(default_state_object.ReleaseAndGetAddressOf(), 0);
	immediate_context->OMSetDepthStencilState(state_object.Get(), 1);
}

void DepthStencilState::deactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->OMSetDepthStencilState(default_state_object.Get(), 1);
}

SamplerState::SamplerState(ID3D11Device* device, D3D11_SAMPLER_DESC sampler_desc)
{
	HRESULT hr = device->CreateSamplerState(&sampler_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

SamplerState::SamplerState(ID3D11Device* device, D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE address_mode, D3D11_COMPARISON_FUNC comparison_func,
	float r, float g, float b, float a)
{
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = filter;
	sampler_desc.AddressU = address_mode;
	sampler_desc.AddressV = address_mode;
	sampler_desc.AddressW = address_mode;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = comparison_func;
	float border_colour[4] = { r, g, b, a };
	memcpy(sampler_desc.BorderColor, border_colour, sizeof(float) * 4);
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = device->CreateSamplerState(&sampler_desc, state_object.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void SamplerState::activate(ID3D11DeviceContext* immediate_context, UINT slot, bool set_in_vs)
{
	using_slot = slot;
	immediate_context->PSGetSamplers(using_slot, 1, cached_state_objects[0].ReleaseAndGetAddressOf());
	immediate_context->VSSetSamplers(using_slot, 1, state_object.GetAddressOf());

	immediate_context->PSSetSamplers(using_slot, 1, state_object.GetAddressOf());
	if (set_in_vs)
	{
		immediate_context->VSGetSamplers(using_slot, 1, cached_state_objects[1].ReleaseAndGetAddressOf());
	}
}
void SamplerState::deactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->PSSetSamplers(using_slot, 1, cached_state_objects[0].GetAddressOf());
	immediate_context->VSSetSamplers(using_slot, 1, cached_state_objects[1].GetAddressOf());
}