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

	D3D11_TEXTURE2D_DESC texture2dDesc;
	load_texture_from_file(device, texture_file_name, shader_resource_view.GetAddressOf(), &texture2dDesc);

	// サブセット用バッファ
	create_constant_buffer(device, subset_constant_buffer.GetAddressOf(), sizeof(CbSubset));

	rasterizer_state = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, false, false);
	depth_stencil_state = std::make_unique<DepthStencilState>(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
	sampler_state = std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS);
}