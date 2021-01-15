#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../NervaLib/high_resolution_timer.h"

class Framework
{
public:
	static Framework& instance(HWND hwnd = nullptr)
	{
		static Framework inst(hwnd);
		return inst;
	}

	int run();

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	ID3D11Device* get_device() const { return device.Get(); }
	ID3D11DeviceContext* get_immediate_context() const { return immediate_context.Get(); }
	ID3D11RenderTargetView* get_render_target_view() const { return onscreen_render_target_view.Get(); }
	ID3D11DepthStencilView* get_depth_stencil_view() const { return onscreen_depth_stencil_view.Get(); }
	D3D11_VIEWPORT get_viewport() const { return viewport; }
	HWND get_hwnd() const { return hwnd; }
	float get_elapsed_time() const { return elapsed_time; }



private:
	const HWND hwnd;
	D3D11_VIEWPORT viewport = {};
	float elapsed_time;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> onscreen_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> onscreen_depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> default_depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> default_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState> default_blend_state;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> default_sampler_state;


	Framework(HWND hwnd) : hwnd(hwnd), elapsed_time(.0f) {}
	~Framework() {}
private:
	bool initialize(bool is_fullscreen = false);
	void update();
	void render();

private:
	HighResolutionTimer timer;
	void calculate_frame_stats();
};