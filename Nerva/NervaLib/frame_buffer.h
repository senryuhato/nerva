#pragma once

#include <d3d11.h>
#include <wrl.h>

class FrameBuffer
{
public:
	FrameBuffer(ID3D11Device* device, UINT width, UINT height);
	FrameBuffer(FrameBuffer&) = delete;
	FrameBuffer& operator= (FrameBuffer&) = delete;

public:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> render_target_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depth_stencil_shader_resource_view;

	D3D11_VIEWPORT viewport = {};

	void clear(ID3D11DeviceContext* immediate_context, float r = .0f, float g = .0f, float b = .0f, float a = 1.0f);
	void activate(ID3D11DeviceContext* immediate_context);
	void deactivate(ID3D11DeviceContext* immediate_context);

private:
	UINT number_of_stored_viewports;
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;

};