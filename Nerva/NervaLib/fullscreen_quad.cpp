#include "fullscreen_quad.h"
#include "shader.h"

FullscreenQuad::FullscreenQuad(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-1, -1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, -1, 0), DirectX::XMFLOAT2(1, 1) },
		{ DirectX::XMFLOAT3(-1, 1, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 0) },
	};

	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	create_vs_from_cso(device, "Shaders/fullscreen_quad_vs.cso", embedded_vertex_shader.GetAddressOf(), embedded_input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "Shaders/fullscreen_quad_ps.cso", embedded_pixel_shader.GetAddressOf());

	embedded_rasterizer_state = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false);
	embedded_depth_stencil_state = std::make_unique<DepthStencilState>(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
	embedded_sampler_state = std::make_unique<SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_BORDER);
}
void FullscreenQuad::blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* shader_resource_view,
	bool use_embedded_rasterizer_state, bool use_embedded_depth_stencil_state, bool use_embedded_pixel_shader)
{
	immediate_context->VSSetShader(embedded_vertex_shader.Get(), nullptr, 0);
	if (use_embedded_pixel_shader) immediate_context->PSSetShader(embedded_pixel_shader.Get(), nullptr, 0);

	if (use_embedded_rasterizer_state) embedded_rasterizer_state->activate(immediate_context);
	if (use_embedded_depth_stencil_state) embedded_depth_stencil_state->activate(immediate_context);
	embedded_sampler_state->activate(immediate_context, 0, 1);


	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(embedded_input_layout.Get());

	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	immediate_context->Draw(4, 0);

	if (use_embedded_rasterizer_state) embedded_rasterizer_state->deactivate(immediate_context);
	if (use_embedded_depth_stencil_state) embedded_depth_stencil_state->deactivate(immediate_context);
	embedded_sampler_state->deactivate(immediate_context);
}