#pragma once

#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fbxsdk.h>

class ModelResource
{
public:
	ModelResource(ID3D11Device* device, const char* fbx_filename, const char* ignore_root_motion_node_name = nullptr);
	~ModelResource() {}

	struct Node
	{
		std::string			name;
		int					parent_index;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
	};

	struct Material
	{
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	};

	struct Subset
	{
		u_int		start_index = 0;
		u_int		index_count = 0;
		Material* material = nullptr;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT2	texcoord;
		DirectX::XMFLOAT4	bone_weight;
		DirectX::XMUINT4	bone_index;
	};

	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;
		std::vector<Subset>						subsets;

		int										node_index;
		std::vector<int>						node_indices;
		std::vector<DirectX::XMFLOAT4X4>		inverse_transforms;
	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	node_keys;
	};
	struct Animation
	{
		float						seconds_length;
		std::vector<Keyframe>		keyframes;
	};

	struct Face
	{
		DirectX::XMFLOAT3 position[3];
	};

	std::vector<Face> faces;

	void add_animation(const char* fbx_filename);

	const std::vector<Mesh>& get_meshes() const { return meshes; }
	const std::vector<Node>& get_nodes() const { return nodes; }
	const std::vector<Animation>& get_animations() const { return animations; }

	void  ray_pick(
		const DirectX::XMFLOAT3& start_position,
		const DirectX::XMFLOAT3& end_position,
		DirectX::XMFLOAT3* out_position,
		DirectX::XMFLOAT3* out_normal,
		float* out_length);

private:
	// ノードデータを構築
	void build_nodes(FbxNode* fbx_node, int parent_node_index);
	void build_node(FbxNode* fbx_node, int parent_node_index);

	// メッシュデータを構築
	void build_meshes(ID3D11Device* device, FbxNode* fbx_node);
	void build_mesh(ID3D11Device* device, FbxNode* fbx_node, FbxMesh* fbx_mesh);

	// マテリアルデータを構築
	void build_materials(ID3D11Device* device, const char* dirname, FbxScene* fbx_scene);
	void build_material(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbx_surface_material);

	// アニメーションデータを構築
	void build_animations(FbxScene* fbx_scene);

	// インデックスの検索
	int find_node_index(const char* name);
	int find_material_index(FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material);

private:
	std::vector<Node>		nodes;
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;
	std::vector<Animation>	animations;
	int						root_motion_node_index = -1;
};
