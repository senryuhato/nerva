#include "model_renderer.h"
#include "../NervaLib/texture.h"

ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// 定数バッファ
	{
		// シーン用バッファ
		create_constant_buffer(device, scene_constant_buffer.GetAddressOf(), sizeof(CbScene));
		// メッシュ用バッファ
		create_constant_buffer(device, mesh_constant_buffer.GetAddressOf(), sizeof(CbMesh));
		// サブセット用バッファ
		create_constant_buffer(device, subset_constant_buffer.GetAddressOf(), sizeof(CbSubset));
	}
	make_dummy_texture(device, dummy_shader_resource_view.GetAddressOf());

	rasterizer_state = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_BACK, false);
	depth_stencil_state = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	sampler_state = std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
}

// 描画開始
void ModelRenderer::begin(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction)
{
	if (!shader) return;

	shader->activate(context);

	ID3D11Buffer* constant_buffers[] =
	{
		scene_constant_buffer.Get(),
		mesh_constant_buffer.Get(),
		subset_constant_buffer.Get()
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);

	rasterizer_state->activate(context);
	depth_stencil_state->activate(context);
	sampler_state->activate(context, 0, false/*set_in_vs*/);

	// シーン用定数バッファ更新
	CbScene cbScene;
	cbScene.view_projection = view_projection;
	cbScene.light_direction = light_direction;
	context->UpdateSubresource(scene_constant_buffer.Get(), 0, 0, &cbScene, 0, 0);
}

// 描画
void ModelRenderer::draw(ID3D11DeviceContext* context, const Model* model)
{
	if (!shader) return;

	const ModelResource* resource = model->get_resource();
	const std::vector<Model::Node>& nodes = model->get_nodes();

	for (const ModelResource::Mesh& mesh : resource->get_meshes())
	{
		// メッシュ用定数バッファ更新
		CbMesh cbMesh;
		::memset(&cbMesh, 0, sizeof(cbMesh));
		if (mesh.node_indices.size() > 0)
		{
			for (size_t i = 0; i < mesh.node_indices.size(); ++i)
			{
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
				DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(&mesh.inverse_transforms.at(i));
				DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
				DirectX::XMStoreFloat4x4(&cbMesh.bone_transforms[i], bone_transform);
			}
		}
		else
		{
			cbMesh.bone_transforms[0] = nodes.at(mesh.node_index).world_transform;
		}
		context->UpdateSubresource(mesh_constant_buffer.Get(), 0, 0, &cbMesh, 0, 0);

		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			CbSubset cbSubset;
			cbSubset.material_color = subset.material->color;
			context->UpdateSubresource(subset_constant_buffer.Get(), 0, 0, &cbSubset, 0, 0);
			context->PSSetShaderResources(0, 1, subset.material->shader_resource_view.Get() ? subset.material->shader_resource_view.GetAddressOf() : dummy_shader_resource_view.GetAddressOf());
			sampler_state->activate(context, 0, false);
			context->DrawIndexed(subset.index_count, subset.start_index, 0);
		}
	}
}

// 描画終了
void ModelRenderer::end(ID3D11DeviceContext* context)
{
	if (!shader) return;

	shader->deactivate(context);

	rasterizer_state->deactivate(context);
	depth_stencil_state->deactivate(context);
	sampler_state->deactivate(context);
}
