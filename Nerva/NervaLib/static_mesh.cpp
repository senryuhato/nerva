#include "shader.h"
#include "texture.h"
#include "misc.h"
#include "static_mesh.h"

#include <fbxsdk.h>
using namespace fbxsdk;

#include <vector>
#include <functional>

void fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

StaticMesh::StaticMesh(ID3D11Device* device, const char* fbxFilename, bool cull_mode)
{
	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool importStatus = false;
	importStatus = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	importStatus = importer->Import(scene);
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetchedMeshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
			if (fbxNodeAttribute)
			{
				switch (fbxNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetchedMeshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	meshes.resize(fetchedMeshes.size());

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.
	for (size_t i = 0; i < fetchedMeshes.size(); i++)
	{
		FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();
		Mesh& mesh = meshes.at(i);

		FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(globalTransform, mesh.global_transform);

		// Fetch material properties.
		const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();

		//subsets.resize(number_of_materials);
		//subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		mesh.subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
		for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
		{
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);

			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);

			const FbxProperty property = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
				if (numberOfTextures)
				{
					const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
					if (fileTexture)
					{
						const char* filename = fileTexture->GetRelativeFileName();
						// Create "diffuse.shader_resource_view" from "filename".
						//wchar_t texture_unicode[256];
						//MultiByteToWideChar(CP_ACP, 0, filename, strlen(filename) + 1, texture_unicode, 1024);
						//D3D11_TEXTURE2D_DESC texture2d_desc;
						//load_texture_from_file(device, texture_unicode, subset.diffuse.shader_resource_view.GetAddressOf(), &texture2d_desc);

						wchar_t fbxUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fbxFilename, static_cast<int>(strlen(fbxFilename) + 1), fbxUnicode, 1024);
						wchar_t textureUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), static_cast<int>(strlen(fileTexture->GetFileName()) + 1), textureUnicode, 1024);
						combine_resource_path(textureUnicode, fbxUnicode, textureUnicode);

						//material.texture_filename = texture_unicode;
						D3D11_TEXTURE2D_DESC texture2dDesc;
						load_texture_from_file(device, textureUnicode, subset.diffuse.shader_resource_view.GetAddressOf(), &texture2dDesc);
					}
				}
			}
		}
		for (Subset& subset : mesh.subsets)
		{
			if (!subset.diffuse.shader_resource_view)
			{
				make_dummy_texture(device, subset.diffuse.shader_resource_view.GetAddressOf());
			}
		}

		// Count the polygon count of each material
		if (numberOfMaterials > 0)
		{
			// Count the faces of each material
			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; ++indexOfPolygon)
			{
				const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);

				//subsets.at(material_index).index_count += 3;
				mesh.subsets.at(materialIndex).index_count += 3;

			}

			// Record the offset (how many vertex)
			int offset = 0;

			//for (subset &subset : subsets)
			for (Subset& subset : mesh.subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;
				// This will be used as counter in the following procedures, reset to zero
				subset.index_count = 0;
			}
		}

		// Fetch mesh data
		std::vector<Vertex> vertices;
		std::vector<u_int> indices;
		u_int vertexCount = 0;

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();
		const int numberOfPolygons = fbxMesh->GetPolygonCount();
		indices.resize(numberOfPolygons * 3);

		for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
		{
			// The material for current face.
			int indexOfMaterial = 0;
			if (numberOfMaterials > 0)
			{
				indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
			}

			// Where should I save the vertex attribute index, according to the material
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);
			const int indexOffset = subset.index_start + subset.index_count;

			//保存用ポリゴンデータ
			Face f;
			f.material_index = indexOfMaterial;//material番号

			for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
			{
				Vertex vertex;
				const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
				vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
				vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
				vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

				DirectX::XMVECTOR local_position = DirectX::XMLoadFloat3(&vertex.position);
				DirectX::XMVECTOR world_transform = DirectX::XMVector3TransformCoord(local_position, DirectX::XMLoadFloat4x4(&mesh.global_transform));

				//面ごとの頂点データ
				DirectX::XMStoreFloat3(&f.position[indexOfVertex], world_transform);


				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				vertices.push_back(vertex);

				indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertexCount);
				vertexCount += 1;
			}
			subset.index_count += 3;

			//面データ保存
			faces.push_back(f);
		}
		//create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
		mesh.create_buffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
	}
	manager->Destroy();

	static_mesh_shader = std::make_unique<StaticmeshShader>(device);

	rasterizer_state = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_BACK, cull_mode);
	depth_stencil_state = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	sampler_state = std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	create_constant_buffer(device, constant_buffer.GetAddressOf(), sizeof(ConstantBuffer));
}

void StaticMesh::Mesh::create_buffers(ID3D11Device* device, Vertex* vertices, int numVertices, u_int* indices, int numIndices)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		bufferDesc.ByteWidth = sizeof(Vertex) * numVertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = device->CreateBuffer(&bufferDesc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = sizeof(u_int) * numIndices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = indices;
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void StaticMesh::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4& world_view_projection, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color)
{
	static_mesh_shader->activate(immediate_context);
	rasterizer_state->activate(immediate_context);
	depth_stencil_state->activate(immediate_context);
	sampler_state->activate(immediate_context, 0, false/*set_in_vs*/);

	for (Mesh& mesh : meshes)
	{
		u_int stride = sizeof(Vertex);
		u_int offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ConstantBuffer data;
		DirectX::XMStoreFloat4x4(&data.world_view_projection, DirectX::XMLoadFloat4x4(&mesh.global_transform) * DirectX::XMLoadFloat4x4(&world_view_projection));
		DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.global_transform) * DirectX::XMLoadFloat4x4(&world));

		data.light_direction = light_direction;
		for (Subset& subset : mesh.subsets)
		{
			data.material_color.x = subset.diffuse.color.x * material_color.x;
			data.material_color.y = subset.diffuse.color.y * material_color.y;
			data.material_color.z = subset.diffuse.color.z * material_color.z;
			data.material_color.w = material_color.w;
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			immediate_context->PSSetShaderResources(0, 1, subset.diffuse.shader_resource_view.GetAddressOf());
			immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}

	static_mesh_shader->deactivate(immediate_context);

	rasterizer_state->deactivate(immediate_context);
	depth_stencil_state->deactivate(immediate_context);
	sampler_state->deactivate(immediate_context);
}

int StaticMesh::ray_pick(
	const DirectX::XMFLOAT3& start_position,
	const DirectX::XMFLOAT3& end_position,
	DirectX::XMFLOAT3* out_position,
	DirectX::XMFLOAT3* out_normal,
	float* out_length,
	float range)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&start_position);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&end_position);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);
	float neart;
	DirectX::XMStoreFloat(&neart, length);

	DirectX::XMVECTOR position, normal;
	for (const auto& it : faces)
	{
		//面頂点取得
		DirectX::XMVECTOR x = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR y = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR z = DirectX::XMLoadFloat3(&it.position[2]);
		//3辺算出
		DirectX::XMVECTOR xy = DirectX::XMVectorSubtract(y, x);
		DirectX::XMVECTOR yz = DirectX::XMVectorSubtract(z, y);
		DirectX::XMVECTOR zx = DirectX::XMVectorSubtract(x, z);
		//外積による法線算出

		DirectX::XMVECTOR n = DirectX::XMVector3Cross(DirectX::XMVector3Normalize(xy), DirectX::XMVector3Normalize(yz));
		//内積の結果がプラスなら裏向き
		float dot;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(n, dir));
		if (dot >= range) continue;
		//交点算出
		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(x, start);
		//DirectX::XMVECTOR a = DirectX::XMVector3Dot(n, v0);
		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(n, v0), DirectX::XMLoadFloat(&dot));
		float ft;
		DirectX::XMStoreFloat(&ft, t);
		if (ft < .0f) continue;
		if (ft > neart) continue;
		DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, DirectX::XMVectorSet(ft, ft, ft, 0.0f)), start);
		//内点判定
		float ab;
		DirectX::XMStoreFloat(&ab, DirectX::XMVector3Dot(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(x, cp), xy), n));
		if (ab < 0) continue;
		float bc;
		DirectX::XMStoreFloat(&bc, DirectX::XMVector3Dot(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(y, cp), yz), n));
		if (bc < 0) continue;
		float ca;
		DirectX::XMStoreFloat(&ca, DirectX::XMVector3Dot(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(z, cp), zx), n));
		if (ca < 0) continue;
		//情報保存
		position = cp;
		normal = n;
		ret = it.material_index;
		out_length = &ft;


	}
	if (ret != -1)
	{
		DirectX::XMStoreFloat3(out_position, position);
		DirectX::XMStoreFloat3(out_normal, normal);
	}
	//最も近いヒット位置までの距離
	*out_length = neart;
	return ret;
}
