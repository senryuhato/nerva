#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "model_resource.h"

class Model
{
public:
	Model(std::shared_ptr<ModelResource>& resource);
	~Model() {}

	struct Node
	{
		const char* name;
		Node* parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	local_transform;
		DirectX::XMFLOAT4X4	world_transform;
	};

	// アニメーション
	bool is_play_animation() const { return current_animation >= 0; }
	void play_animation(int animation_index, bool loop = false);
	void update_animation(float elapsed_time);

	// 行列計算
	void calculate_local_transform();
	void calculate_world_transform(const DirectX::XMMATRIX& world_transform);

	const std::vector<Node>& get_nodes() const { return nodes; }
	std::vector<Node>& get_nodes() { return nodes; }
	const ModelResource* get_resource() const { return resource.get(); }

// private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;
	int								current_animation = -1;
	float							current_seconds = 0.0f;
	bool							loop_animation = false;
	bool							end_animation = false;
};
