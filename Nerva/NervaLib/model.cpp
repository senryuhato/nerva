#include "misc.h"
#include "model.h"

Model::Model(std::shared_ptr<ModelResource>& resource)
{
	this->resource = resource;

	// �m�[�h
	const std::vector<ModelResource::Node>& res_nodes = resource->get_nodes();

	nodes.resize(res_nodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = res_nodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}
}

// �A�j���[�V�����Đ�
void Model::play_animation(int animationIndex, bool loop)
{
	current_animation = animationIndex;
	loop_animation = loop;
	end_animation = false;
	current_seconds = 0.0f;
}

// �A�j���[�V�����v�Z
void Model::update_animation(float elapsed_time)
{
	if (current_animation < 0) return;
	if (resource->get_animations().empty()) return;

	const ModelResource::Animation& animation = resource->get_animations().at(current_animation);

	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int key_count = static_cast<int>(keyframes.size());
	for (int key_index = 0; key_index < key_count - 1; ++key_index)
	{
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const ModelResource::Keyframe& keyframe0 = keyframes.at(key_index);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(key_index + 1); 
		if (current_seconds >= keyframe0.seconds && current_seconds < keyframe1.seconds)
		{
			float rate = (current_seconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			assert(nodes.size() == keyframe0.node_keys.size());
			assert(nodes.size() == keyframe1.node_keys.size());
			int node_count = static_cast<int>(nodes.size());
			for (int node_index = 0; node_index < node_count; ++node_index)
			{
				// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
				const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
				const ModelResource::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

				Node& node = nodes[node_index];

				DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
				DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
				DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
				DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

				DirectX::XMStoreFloat3(&node.scale, s);
				DirectX::XMStoreFloat4(&node.rotate, r);
				DirectX::XMStoreFloat3(&node.translate, t);
			}
			break;
		}
	}

	// �ŏI�t���[������
	if (end_animation)
	{
		end_animation = false;
		current_animation = -1;
		return;
	}

	// ���Ԍo��
	current_seconds += elapsed_time;
	if (current_seconds >= animation.seconds_length)
	{
		if (loop_animation)
		{
			current_seconds -= animation.seconds_length;
		}
		else
		{
			current_seconds = animation.seconds_length;
			end_animation = true;
		}
	}
}

// ���[�J���ϊ��s��v�Z
void Model::calculate_local_transform()
{
	for (Node& node : nodes)
	{
		DirectX::XMMATRIX scale, rotate, translate;
		scale = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		rotate = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotate.x, node.rotate.y, node.rotate.z, node.rotate.w));
		translate = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);

		DirectX::XMStoreFloat4x4(&node.local_transform, scale * rotate * translate);
	}
}

// ���[���h�ϊ��s��v�Z
void Model::calculate_world_transform(const DirectX::XMMATRIX& world_transform)
{
	for (Node& node : nodes)
	{
		DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.local_transform);
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX parent_transform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
			DirectX::XMStoreFloat4x4(&node.world_transform, local_transform * parent_transform);
		}
		else
		{
			DirectX::XMStoreFloat4x4(&node.world_transform, local_transform * world_transform);
		}
	}
}
