#include "sprite.h"

#include "misc.h"
#include "texture.h"

Sprite::Sprite(ID3D11Device* device, const wchar_t* texture_file_name)
{
	HRESULT hr;
	{
		Vertex vertices[4] = {};
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(vertices);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = vertices;
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bd, &init_data, &vertex_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	load_texture_from_file(device, texture_file_name, shader_resource_view.GetAddressOf(), &texture2d_desc);

	// サブセット用バッファ
	create_constant_buffer(device, subset_constant_buffer.GetAddressOf(), sizeof(CbSubset));

	depth_stencil_state = std::make_unique<DepthStencilState>(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
	sampler_state = std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS);
	rasterizer_state = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, false, false);
}

void Sprite::render(ID3D11DeviceContext* immediate_context,
	DirectX::XMFLOAT2 position,
	DirectX::XMFLOAT2 scale,
	float angle,
	DirectX::XMFLOAT2 size,
	DirectX::XMFLOAT2 tex_pos,
	DirectX::XMFLOAT2 tex_size,
	DirectX::XMFLOAT2 center,
	DirectX::XMFLOAT4 material_color
)
{
	if (!shader) return;
	//準備処理
	shader->activate(immediate_context);
	rasterizer_state->activate(immediate_context);
	depth_stencil_state->activate(immediate_context);
	sampler_state->activate(immediate_context, 0, false/*set_in_vs*/);


	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	immediate_context->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	float cx = -size.x * center.x;
	float cy = -size.y * center.y;
	Vertex vertices[] =
	{
		{{cx, cy, .0f},                   {tex_pos.x,tex_pos.y}},
		{{cx, cy + size.y, .0f},          {tex_pos.x,tex_pos.y + tex_size.y}},
		{{cx + size.x, cy, .0f},          {tex_pos.x + tex_size.x,tex_pos.y}},
		{{cx + size.x, cy + size.y, .0f}, {tex_pos.x + tex_size.x,tex_pos.y + tex_size.y}},
	};

	for (int i = 0; i < 4; i++)
	{
		vertices[i].position.x *= scale.x;
		vertices[i].position.y *= scale.y;

		float pos_x = vertices[i].position.x;
		float pos_y = vertices[i].position.y;
		vertices[i].position.x = cosf(angle) * pos_x + sinf(angle) * pos_y;
		vertices[i].position.y = sinf(angle) * pos_x - cosf(angle) * pos_y;

		vertices[i].position.x += position.x;
		vertices[i].position.y += position.y;

		vertices[i].position.x = (vertices[i].position.x / screen_width) * 2.0f - 1.0f;
		vertices[i].position.y = (vertices[i].position.y / screen_height) * 2.0f + 1.0f;

		vertices[i].texcoord.x = vertices[i].texcoord.x / texture2d_desc.Width;
		vertices[i].texcoord.y = vertices[i].texcoord.y / texture2d_desc.Height;

	}


	D3D11_MAPPED_SUBRESOURCE msr = {};
	immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	immediate_context->Unmap(vertex_buffer.Get(), 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	CbSubset cb_subset;
	cb_subset.material_color.x = material_color.x;
	cb_subset.material_color.y = material_color.y;
	cb_subset.material_color.z = material_color.z;
	cb_subset.material_color.w = material_color.w;
	immediate_context->UpdateSubresource(subset_constant_buffer.Get(), 0, 0, &cb_subset, 0, 0);
	immediate_context->VSSetConstantBuffers(2, 1, subset_constant_buffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(2, 1, subset_constant_buffer.GetAddressOf());

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	immediate_context->Draw(4, 0);


	//終了処理
	rasterizer_state->deactivate(immediate_context);
	depth_stencil_state->deactivate(immediate_context);
	sampler_state->deactivate(immediate_context);

	shader->deactivate(immediate_context);
}

