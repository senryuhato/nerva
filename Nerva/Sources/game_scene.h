#pragma once

#include "../NervaLib/render_state.h"
#include "../NervaLib/frame_buffer.h"
#include "../NervaLib/fullscreen_quad.h"

#include "../NervaLib/model_resource.h"
#include "../NervaLib/model.h"

#include "../NervaLib/sprite.h"
#include "../NervaLib/static_mesh.h"

#include <d3d11.h>

#include <thread>
#include <mutex>

#include "scene.h"
#include "model_renderer.h"
#include "collision.h"
#include "camera.h"
#include "TPCamera.h"
#include "player_object.h"
#include "boss_object.h"

class GameScene : public Scene
{
public:
	//描画関係
	std::unique_ptr<FrameBuffer> frame_buffer;
	std::unique_ptr<FullscreenQuad> fullscreen_quad;

	//state
	std::unique_ptr<BlendState> blend_state;

	//カメラ
	std::unique_ptr<TPCamera> camera;

	//shader
	std::shared_ptr<Shader> lambert_shader;
	std::shared_ptr<Shader> sprite_shader;

	//プレイヤー
	std::shared_ptr<ModelResource> player_model_resource;
	std::shared_ptr<Model> player_model;
	std::shared_ptr<ModelRenderer> player_model_renderer;
	std::shared_ptr<PlayerObject> player_object;

	//boss
	std::shared_ptr<ModelResource> boss_model_resource;
	std::shared_ptr<Model> boss_model;
	std::shared_ptr<ModelRenderer> boss_model_renderer;
	std::shared_ptr<BossObject> boss_object;

	//static
	std::shared_ptr<StaticMesh> ground_mesh;
	std::shared_ptr<StaticObject> ground_object;

	std::shared_ptr<StaticMesh> ground_collision_mesh;
	std::shared_ptr<StaticObject> ground_collision_object;

	//collision
	std::shared_ptr<Collision> ground_collision;

	//sprite
	std::unique_ptr<Sprite> sprite;

	//hit
	std::unique_ptr<Hit> hit;

public:
	void initialize();
	void update();
	void render();
	void uninitialize();

public:
	std::mutex loading_mutex;
	std::unique_ptr<std::thread> loading_thread;
	bool now_loading = true;
	static void loading_thread_function(GameScene* scene, ID3D11Device* device);
	bool is_now_loading()
	{
		if (loading_thread && !now_loading && loading_mutex.try_lock())
		{
			loading_mutex.unlock();
			return false;
		}
		return true;
	}
	~GameScene()
	{
		if (loading_thread && loading_thread->joinable()) loading_thread->join();
		uninitialize();
	}
};