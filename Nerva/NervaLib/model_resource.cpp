#include <stdlib.h>
#include <functional>

#include "texture.h"
#include "misc.h"
#include "model_resource.h"

// FbxDouble2 → XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbxValue)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1])
	);
}

// FbxDouble3 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2]),
		static_cast<float>(fbxValue[3])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbxValue)
{
	return DirectX::XMFLOAT4X4(
		static_cast<float>(fbxValue[0][0]),
		static_cast<float>(fbxValue[0][1]),
		static_cast<float>(fbxValue[0][2]),
		static_cast<float>(fbxValue[0][3]),
		static_cast<float>(fbxValue[1][0]),
		static_cast<float>(fbxValue[1][1]),
		static_cast<float>(fbxValue[1][2]),
		static_cast<float>(fbxValue[1][3]),
		static_cast<float>(fbxValue[2][0]),
		static_cast<float>(fbxValue[2][1]),
		static_cast<float>(fbxValue[2][2]),
		static_cast<float>(fbxValue[2][3]),
		static_cast<float>(fbxValue[3][0]),
		static_cast<float>(fbxValue[3][1]),
		static_cast<float>(fbxValue[3][2]),
		static_cast<float>(fbxValue[3][3])
	);
}

ModelResource::ModelResource(ID3D11Device* device, const char* fbx_filename, const char* ignore_root_motion_node_name)
{
	FbxManager* fbx_manager = FbxManager::Create();

	// FBXに対する入出力を定義する
	FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbx_manager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
	fbx_manager->SetIOSettings(fbxIOS);

	// インポータを生成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	bool result = fbx_importer->Initialize(fbx_filename, -1, fbx_manager->GetIOSettings());	// -1でファイルフォーマット自動判定
	_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

	// SceneオブジェクトにFBXファイル内の情報を流し込む
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "scene");
	fbx_importer->Import(fbx_scene);
	fbx_importer->Destroy();	// シーンを流し込んだらImporterは解放してOK

	// ジオメトリを三角形化しておく
	FbxGeometryConverter fbxGeometryConverter(fbx_manager);
	fbxGeometryConverter.Triangulate(fbx_scene, true);
	fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbx_scene);

	// ディレクトリパス取得
	char dirname[256];
	::_splitpath_s(fbx_filename, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

	// モデル構築
	FbxNode* fbx_root_node = fbx_scene->GetRootNode();
	build_materials(device, dirname, fbx_scene);
	build_nodes(fbx_root_node, -1);
	build_meshes(device, fbx_root_node);

	// 無視するルートモーションを検索
	if (ignore_root_motion_node_name != nullptr)
	{
		root_motion_node_index = -1;
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes.at(i).name == ignore_root_motion_node_name)
			{
				root_motion_node_index = static_cast<int>(i);
				break;
			}
		}
	}

	// アニメーション構築
	build_animations(fbx_scene);

	// マネージャ解放
	fbx_manager->Destroy();		// 関連するすべてのオブジェクトが解放される
}

void ModelResource::add_animation(const char* fbx_filename)
{
	FbxManager* fbx_manager = FbxManager::Create();

	// FBXに対する入出力を定義する
	FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbx_manager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
	fbx_manager->SetIOSettings(fbxIOS);

	// インポータを生成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	bool result = fbx_importer->Initialize(fbx_filename, -1, fbx_manager->GetIOSettings());	// -1でファイルフォーマット自動判定
	_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

	// SceneオブジェクトにFBXファイル内の情報を流し込む
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "scene");
	fbx_importer->Import(fbx_scene);
	fbx_importer->Destroy();	// シーンを流し込んだらImporterは解放してOK

	// アニメーション構築
	build_animations(fbx_scene);

	// マネージャ解放
	fbx_manager->Destroy();		// 関連するすべてのオブジェクトが解放される
}

// FBXノードを再帰的に辿ってデータを構築する
void ModelResource::build_nodes(FbxNode* fbx_node, int parent_node_index)
{
	FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
	FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

	if (fbx_node_attribute != nullptr)
	{
		fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
	}

	switch (fbx_node_attribute_type)
	{
	case FbxNodeAttribute::eUnknown:
	case FbxNodeAttribute::eNull:
	case FbxNodeAttribute::eMarker:
	case FbxNodeAttribute::eMesh:
	case FbxNodeAttribute::eSkeleton:
		build_node(fbx_node, parent_node_index);
		break;
	}

	// 再帰的に子ノードを処理する
	parent_node_index = static_cast<int>(nodes.size() - 1);
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		build_nodes(fbx_node->GetChild(i), parent_node_index);
	}
}

// FBXノードからノードデータを構築する
void ModelResource::build_node(FbxNode* fbx_node, int parent_node_index)
{
	FbxAMatrix& fbx_local_transform = fbx_node->EvaluateLocalTransform();

	Node node;
	node.name = fbx_node->GetName();
	node.parent_index = parent_node_index;
	node.scale = FbxDouble4ToFloat3(fbx_local_transform.GetS());
	node.rotate = FbxDouble4ToFloat4(fbx_local_transform.GetQ());
	node.translate = FbxDouble4ToFloat3(fbx_local_transform.GetT());

	nodes.emplace_back(node);
}

// FBXノードを再帰的に辿ってメッシュデータを構築する
void ModelResource::build_meshes(ID3D11Device* device, FbxNode* fbx_node)
{
	FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
	FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

	if (fbx_node_attribute != nullptr)
	{
		fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
	}

	switch (fbx_node_attribute_type)
	{
	case FbxNodeAttribute::eMesh:
		build_mesh(device, fbx_node, static_cast<FbxMesh*>(fbx_node_attribute));
		break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		build_meshes(device, fbx_node->GetChild(i));
	}
}

// FBXメッシュからメッシュデータを構築する
void ModelResource::build_mesh(ID3D11Device* device, FbxNode* fbx_node, FbxMesh* fbx_mesh)
{
	int fbx_control_points_count = fbx_mesh->GetControlPointsCount();
	//int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbx_material_count = fbx_node->GetMaterialCount();
	int fbx_polygon_count = fbx_mesh->GetPolygonCount();

	meshes.emplace_back(Mesh());
	Mesh& mesh = meshes.back();
	mesh.subsets.resize(fbx_material_count > 0 ? fbx_material_count : 1);
	mesh.node_index = find_node_index(fbx_node->GetName());

	// サブセットのマテリアル設定
	for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
	{
		const FbxSurfaceMaterial* fbx_surface_material = fbx_node->GetMaterial(fbx_material_index);

		Subset& subset = mesh.subsets.at(fbx_material_index);
		int index = find_material_index(fbx_node->GetScene(), fbx_surface_material);
		subset.material = &materials.at(index);
	}

	// サブセットの頂点インデックス範囲設定
	if (fbx_material_count > 0)
	{
		for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
		{
			int fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
			mesh.subsets.at(fbx_material_index).index_count += 3;
		}

		int offset = 0;
		for (Subset& subset : mesh.subsets)
		{
			subset.start_index = offset;
			offset += subset.index_count;

			subset.index_count = 0;
		}
	}

	// 頂点影響力データ
	struct BoneInfluence
	{
		int use_count = 0;
		int indices[4] = { 0, 0, 0, 0 };
		float weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

		void Add(int index, float weight)
		{
			if (use_count < 4)
			{
				indices[use_count] = index;
				weights[use_count] = weight;
				use_count++;
			}
		}
	};
	// 頂点影響力データを抽出する
	std::vector<BoneInfluence> bone_influences;
	{
		bone_influences.resize(fbx_control_points_count);

		FbxAMatrix fbx_geometric_transform(
			fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricRotation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricScaling(FbxNode::eSourcePivot)
		);

		// スキニングに必要な情報を取得する
		int fbx_deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbx_deformer_index = 0; fbx_deformer_index < fbx_deformer_count; ++fbx_deformer_index)
		{
			FbxSkin* fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(fbx_deformer_index, FbxDeformer::eSkin));

			int fbx_cluster_count = fbx_skin->GetClusterCount();
			for (int fbx_cluster_index = 0; fbx_cluster_index < fbx_cluster_count; ++fbx_cluster_index)
			{
				FbxCluster* fbx_cluster = fbx_skin->GetCluster(fbx_cluster_index);

				// 頂点影響力データを抽出する
				{
					int fbx_cluster_control_point_indices_count = fbx_cluster->GetControlPointIndicesCount();
					const int* fbx_control_point_indices = fbx_cluster->GetControlPointIndices();
					const double* fbx_control_point_weights = fbx_cluster->GetControlPointWeights();

					for (int i = 0; i < fbx_cluster_control_point_indices_count; ++i)
					{
						BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_indices[i]);

						bone_influence.Add(fbx_cluster_index, static_cast<float>(fbx_control_point_weights[i]));
					}
				}

				// ボーン変換行列用の逆行列の計算をする
				{
					// メッシュ空間からワールド空間への変換行列
					FbxAMatrix fbx_mesh_space_transform;
					fbx_cluster->GetTransformMatrix(fbx_mesh_space_transform);

					// ボーン空間からワールド空間への変換行列
					FbxAMatrix fbx_bone_space_transform;
					fbx_cluster->GetTransformLinkMatrix(fbx_bone_space_transform);

					// ボーン逆行列を計算する
					FbxAMatrix fbxInverseTransform = fbx_bone_space_transform.Inverse() * fbx_mesh_space_transform * fbx_geometric_transform;

					DirectX::XMFLOAT4X4 inverseTransform = FbxAMatrixToFloat4x4(fbxInverseTransform);
					mesh.inverse_transforms.emplace_back(inverseTransform);

					int nodeIndex = find_node_index(fbx_cluster->GetLink()->GetName());
					mesh.node_indices.emplace_back(nodeIndex);
				}
			}
		}
	}

	// UVセット名
	FbxStringList fbx_uv_set_names;
	fbx_mesh->GetUVSetNames(fbx_uv_set_names);

	// 頂点データ
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	vertices.resize(fbx_polygon_count * 3);
	indices.resize(fbx_polygon_count * 3);

	FbxAMatrix globalTransform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
	DirectX::XMFLOAT4X4 global_transform = {};
	global_transform = FbxAMatrixToFloat4x4(globalTransform);

	int vertex_count = 0;
	const FbxVector4* fbx_control_points = fbx_mesh->GetControlPoints();
	for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
		int fbx_material_index = 0;
		if (fbx_material_count > 0)
		{
			fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
		}

		Subset& subset = mesh.subsets.at(fbx_material_index);
		const int index_offset = subset.start_index + subset.index_count;

		//保存用ポリゴンデータ
		Face f;

		for (int fbx_vertex_index = 0; fbx_vertex_index < 3; ++fbx_vertex_index)
		{
			Vertex vertex;

			int fbx_control_point_index = fbx_mesh->GetPolygonVertex(fbx_polygon_index, fbx_vertex_index);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbx_control_points[fbx_control_point_index]);
			
				DirectX::XMVECTOR local_position = DirectX::XMLoadFloat3(&vertex.position);
				DirectX::XMVECTOR world_transform = DirectX::XMVector3TransformCoord(local_position, DirectX::XMLoadFloat4x4(&global_transform));

				//面ごとの頂点データ
				f.position[fbx_vertex_index] = vertex.position;
			}

			// Weight
			{
				BoneInfluence& boneInfluence = bone_influences.at(fbx_control_point_index);
				vertex.bone_index.x = boneInfluence.indices[0];
				vertex.bone_index.y = boneInfluence.indices[1];
				vertex.bone_index.z = boneInfluence.indices[2];
				vertex.bone_index.w = boneInfluence.indices[3];
				vertex.bone_weight.x = boneInfluence.weights[0];
				vertex.bone_weight.y = boneInfluence.weights[1];
				vertex.bone_weight.z = boneInfluence.weights[2];
				vertex.bone_weight.w = 1.0f - (vertex.bone_weight.x + vertex.bone_weight.y + vertex.bone_weight.z);
				//vertex.boneWeight.w = boneInfluence.weights[3];
			}

			// Normal
			if (fbx_mesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbx_normal;
				fbx_mesh->GetPolygonVertexNormal(fbx_polygon_index, fbx_vertex_index, fbx_normal);
				vertex.normal = FbxDouble4ToFloat3(fbx_normal);
			}
			else
			{
				vertex.normal = DirectX::XMFLOAT3(0, 0, 0);
			}

			// Texcoord
			if (fbx_mesh->GetElementUVCount() > 0)
			{
				bool fbx_unmapped_uv;
				FbxVector2 fbx_uv;
				fbx_mesh->GetPolygonVertexUV(fbx_polygon_index, fbx_vertex_index, fbx_uv_set_names[0], fbx_uv, fbx_unmapped_uv);
				fbx_uv[1] = 1.0 - fbx_uv[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbx_uv);
			}
			else
			{
				vertex.texcoord = DirectX::XMFLOAT2(0, 0);
			}

			indices.at(index_offset + fbx_vertex_index) = vertex_count;
			vertices.at(vertex_count) = vertex;
			vertex_count++;
		}

		subset.index_count += 3;

		//面データ保存
		faces.push_back(f);
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.data();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertex_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// インデックスバッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * indices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = indices.data();
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.index_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

// FBXシーン内のFBXマテリアルからマテリアルデータを構築する
void ModelResource::build_materials(ID3D11Device* device, const char* dirname, FbxScene* fbx_scene)
{
	int fbxMaterialCount = fbx_scene->GetMaterialCount();

	if (fbxMaterialCount > 0)
	{
		for (int fbx_material_index = 0; fbx_material_index < fbxMaterialCount; ++fbx_material_index)
		{
			FbxSurfaceMaterial* fbx_surface_material = fbx_scene->GetMaterial(fbx_material_index);

			build_material(device, dirname, fbx_surface_material);
		}
	}
	else
	{
		Material material;
		material.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		materials.emplace_back(material);
	}
}

// FBXマテリアルからマテリアルデータを構築する
void ModelResource::build_material(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	bool ret = false;

	Material material;

	// ディフューズカラー
	FbxProperty fbxDiffuseProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxProperty fbxDiffuseFactorProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	if (fbxDiffuseProperty.IsValid() && fbxDiffuseFactorProperty.IsValid())
	{
		FbxDouble fbxFactor = fbxDiffuseFactorProperty.Get<FbxDouble>();
		FbxDouble3 fbxColor = fbxDiffuseProperty.Get<FbxDouble3>();

		material.color.x = static_cast<float>(fbxColor[0] * fbxFactor);
		material.color.y = static_cast<float>(fbxColor[1] * fbxFactor);
		material.color.z = static_cast<float>(fbxColor[2] * fbxFactor);
		material.color.w = 1.0f;
	}
	// ディフューズテクスチャ
	if (fbxDiffuseProperty.IsValid())
	{
		int fbxTextureCount = fbxDiffuseProperty.GetSrcObjectCount<FbxFileTexture>();
		if (fbxTextureCount > 0)
		{
			FbxFileTexture* fbxTexture = fbxDiffuseProperty.GetSrcObject<FbxFileTexture>();

			// 相対パスの解決
			char filename[256];
			::_makepath_s(filename, 256, nullptr, dirname, fbxTexture->GetRelativeFileName(), nullptr);

			// マルチバイト文字からワイド文字へ変換
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, filename, _TRUNCATE);

			// テクスチャ読み込み
			D3D11_TEXTURE2D_DESC texture2dDesc;
			load_texture_from_file(device, wfilename, material.shader_resource_view.GetAddressOf(), &texture2dDesc);
		}
	}

	materials.emplace_back(material);
}

// アニメーションデータを構築
void ModelResource::build_animations(FbxScene* fbxScene)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);

	int fbxAnimationCount = fbxAnimStackNames.Size();
	for (int fbxAnimationIndex = 0; fbxAnimationIndex < fbxAnimationCount; ++fbxAnimationIndex)
	{
		animations.emplace_back(Animation());
		Animation& animation = animations.back();

		// アニメーションデータのサンプリング設定
		FbxTime::EMode fbxTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime fbxFrameTime;
		fbxFrameTime.SetTime(0, 0, 0, 1, 0, fbxTimeMode);

		float samplingRate = static_cast<float>(fbxFrameTime.GetFrameRate(fbxTimeMode));
		float samplingTime = 1.0f / samplingRate;

		FbxString* fbxAnimStackName = fbxAnimStackNames.GetAt(fbxAnimationIndex);
		FbxAnimStack* fbxAnimStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackName->Buffer());

		// 再生するアニメーションを指定する。
		fbxScene->SetCurrentAnimationStack(fbxAnimStack);

		// アニメーションの再生開始時間と再生終了時間を取得する
		FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName->Buffer());
		FbxTime fbxStartTime = fbxTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime fbxEndTime = fbxTakeInfo->mLocalTimeSpan.GetStop();

		// 抽出するデータは60フレーム基準でサンプリングする
		FbxTime fbxSamplingStep;
		fbxSamplingStep.SetTime(0, 0, 1, 0, 0, fbxTimeMode);
		fbxSamplingStep = static_cast<FbxLongLong>(fbxSamplingStep.Get() * samplingTime);

		int startFrame = static_cast<int>(fbxStartTime.Get() / fbxSamplingStep.Get());
		int endFrame = static_cast<int>(fbxEndTime.Get() / fbxSamplingStep.Get());
		int frameCount = static_cast<int>((fbxEndTime.Get() - fbxStartTime.Get()) / fbxSamplingStep.Get());

		// アニメーションの対象となるノードを列挙する
		std::vector<FbxNode*> fbxNodes;
		FbxNode* fbxRootNode = fbxScene->GetRootNode();
		for (Node& node : nodes)
		{
			FbxNode* fbxNode = fbxRootNode->FindChild(node.name.c_str());
			fbxNodes.emplace_back(fbxNode);
		}

		// アニメーションデータを抽出する
		animation.seconds_length = frameCount * samplingTime;
		animation.keyframes.resize(frameCount + 1);

		float seconds = 0.0f;
		Keyframe* keyframe = animation.keyframes.data();
		size_t fbxNodeCount = fbxNodes.size();
		FbxTime fbxCurrentTime = fbxStartTime;
		for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime < fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyframe)
		{
			// キーフレーム毎の姿勢データを取り出す。
			keyframe->seconds = seconds;
			keyframe->node_keys.resize(fbxNodeCount);
			for (size_t fbxNodeIndex = 0; fbxNodeIndex < fbxNodeCount; ++fbxNodeIndex)
			{
				NodeKeyData& keyData = keyframe->node_keys.at(fbxNodeIndex);
				FbxNode* fbxNode = fbxNodes.at(fbxNodeIndex);
				if (fbxNode == nullptr)
				{
					// アニメーション対象のノードがなかったのでダミーデータを設定
					Node& node = nodes.at(fbxNodeIndex);
					keyData.scale = node.scale;
					keyData.rotate = node.rotate;
					keyData.translate = node.translate;
				}
				else if (fbxNodeIndex == root_motion_node_index)
				{
					// ルートモーションは無視する
					Node& node = nodes.at(fbxNodeIndex);
					keyData.scale = DirectX::XMFLOAT3(1, 1, 1);
					keyData.rotate = DirectX::XMFLOAT4(0, 0, 0, 1);
					keyData.translate = DirectX::XMFLOAT3(0, 0, 0);
				}
				else
				{
					// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
					const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);

					keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
					keyData.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
					keyData.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
				}
			}
			seconds += samplingTime;
		}
	}

	// 後始末
	for (int i = 0; i < fbxAnimationCount; i++)
	{
		delete fbxAnimStackNames[i];
	}
}

// ノードインデックスを取得する
int ModelResource::find_node_index(const char* name)
{
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// マテリアルインデックスを取得する
int ModelResource::find_material_index(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	int fbxMaterialCount = fbxScene->GetMaterialCount();

	for (int i = 0; i < fbxMaterialCount; ++i)
	{
		if (fbxScene->GetMaterial(i) == fbxSurfaceMaterial)
		{
			return i;
		}
	}
	return -1;
}

void  ModelResource::ray_pick(
	const DirectX::XMFLOAT3& start_position,
	const DirectX::XMFLOAT3& end_position,
	DirectX::XMFLOAT3* out_position,
	DirectX::XMFLOAT3* out_normal,
	float* out_length)
{
	bool ret = false;
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
		if (dot >= 0) continue;
		//交点算出
		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(x, start);
		DirectX::XMVECTOR a = DirectX::XMVector3Dot(n, v0);
		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(n, v0), DirectX::XMLoadFloat(&dot));
		float ft;
		DirectX::XMStoreFloat(&ft, t);
		if (ft < .0f || ft > neart) continue;
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
		ret = true;
		out_length = &ft;
	}
	if (ret)
	{
		DirectX::XMStoreFloat3(out_position, position);
		DirectX::XMStoreFloat3(out_normal, normal);
	}
	//最も近いヒット位置までの距離
	*out_length = neart;
}

