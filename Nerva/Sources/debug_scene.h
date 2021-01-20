#pragma once

#include "../NervaLib/render_state.h"
#include "../NervaLib/frame_buffer.h"
#include "../NervaLib/fullscreen_quad.h"

#include "../NervaLib/model_resource.h"
#include "../NervaLib/model.h"

#include "../NervaLib/sprite.h"
#include "../NervaLib/static_mesh.h"

#include "scene.h"
#include "camera.h"
#include "model_renderer.h"
#include "player_object.h"
#include "static_object.h"
#include "collision.h"
#include "boss_object.h"

#include <d3d11.h>

#include <thread>
#include <mutex>

class DebugScene : public Scene
{
public:
	//�`��֌W
	std::unique_ptr<FrameBuffer> frame_buffer;
	std::unique_ptr<FullscreenQuad> fullscreen_quad;

	//state
	std::unique_ptr<BlendState> blend_state;

	//�J����
	std::unique_ptr<Camera> camera;

	//shader
	std::shared_ptr<Shader> lambert_shader;
	std::shared_ptr<Shader> sprite_shader;

	//�v���C���[
	std::shared_ptr<ModelResource> player_model_resource;
	std::shared_ptr<Model> player_model;
	std::shared_ptr<ModelRenderer> player_model_renderer;
	std::unique_ptr<PlayerObject> player_object;

	//boss
	std::shared_ptr<ModelResource> boss_model_resource;
	std::shared_ptr<Model> boss_model;
	std::shared_ptr<ModelRenderer> boss_model_renderer;
	std::unique_ptr<BossObject> boss_object;

	//static
	std::shared_ptr<StaticMesh> ground_mesh;
	std::shared_ptr<StaticObject> ground_object;

	//collision
	std::shared_ptr<Collision> ground_collision;

	//sprite
	std::unique_ptr<Sprite> sprite;

public:
	void initialize();
	void update();
	void render();
	void uninitialize();

public:
	std::mutex loading_mutex;
	std::unique_ptr<std::thread> loading_thread;
	bool now_loading = true;
	static void loading_thread_function(DebugScene* scene, ID3D11Device* device);
	bool is_now_loading()
	{
		if (loading_thread && !now_loading && loading_mutex.try_lock())
		{
			loading_mutex.unlock();
			return false;
		}
		return true;
	}
	~DebugScene()
	{
		if (loading_thread && loading_thread->joinable()) loading_thread->join();
		uninitialize();
	}
};
