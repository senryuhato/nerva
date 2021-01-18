#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <memory>

#include "shader.h"
#include "texture.h"
#include "render_state.h"

class StaticmeshShader : public Shader
{
public:
	StaticmeshShader(ID3D11Device* device)
	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		vs_name = "Shaders/static_mesh_vs.cso";
		ps_name = "Shaders/static_mesh_ps.cso";

		create_vs_from_cso(device, vs_name.c_str(), vertex_shader.GetAddressOf(),
			input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device, ps_name.c_str(), pixel_shader.GetAddressOf());
	}
};

class StaticMesh
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Material
	{
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f }; // w channel is used as shininess by only specular.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	};

	struct Subset
	{
		u_int index_start = 0;	// start number of index buffer
		u_int index_count = 0;	// number of vertices (indices)
		Material diffuse;
	};

	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<Subset> subsets;

		DirectX::XMFLOAT4X4 global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		void create_buffers(ID3D11Device* device, Vertex* vertices, int num_vertices, u_int* indices, int num_indices);
	};
	std::vector<Mesh> meshes;

	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	std::unique_ptr<StaticmeshShader> static_mesh_shader;

	std::unique_ptr<RasterizerState> rasterizer_state;
	std::unique_ptr<DepthStencilState> depth_stencil_state;
	std::unique_ptr<SamplerState> sampler_state;

public:
	class StaticMesh(ID3D11Device* device, const char* fbxFilename);
	virtual ~StaticMesh() {}

	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world_view_projection,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& light_direction,
		const DirectX::XMFLOAT4& material_color);


private:
	//レイピック用ポリゴン
	struct Face
	{
		DirectX::XMFLOAT3 position[3];
		int material_index;
	};
	std::vector<Face> faces;

public:
	//レイピック関数
	int ray_pick(
		const DirectX::XMFLOAT3& start_position,
		const DirectX::XMFLOAT3& end_position,
		DirectX::XMFLOAT3* out_position,
		DirectX::XMFLOAT3* out_normal,
		float* out_length
	);
};
