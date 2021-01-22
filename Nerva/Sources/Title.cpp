#include "../NervaLib/key_input.h"
#include "framework.h"
#include "Title.h"

void TitleScene::loading_thread_function(TitleScene* scene, ID3D11Device* device)
{
	std::lock_guard<std::mutex> lock(scene->loading_mutex);

	scene->now_loading = false;
}

void TitleScene::initialize()
{
	ID3D11Device* device = Framework::instance().get_device();
	D3D11_VIEWPORT view_port = Framework::instance().get_viewport();

	frame_buffer = std::make_unique<FrameBuffer>(device, (UINT)view_port.Width, (UINT)view_port.Height);
	fullscreen_quad = std::make_unique<FullscreenQuad>(device);

	//ブレンドモード
	blend_state = std::make_unique<BlendState>(device, BlendState::ALPHA);

	//カメラ
	camera = std::make_unique<Camera>();
	camera->initialize();

	//shader
	lambert_shader = std::make_shared<LambertShader>(device);
	sprite_shader = std::make_shared<SpriteShader>(device);

	//プレイヤー
	/*player_model_renderer = std::make_shared<ModelRenderer>(device);
	player_model_renderer->set_shader(lambert_shader);

	player_model_resource = std::make_shared<ModelResource>(device, "Data/Player/001.fbx");

	player_model = std::make_shared<Model>(player_model_resource);
	player_model->play_animation(0, true);

	player_object = std::make_unique<PlayerObject>();
	player_object->initialize();
	player_object->transform.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	player_object->set_model(player_model);
	player_object->set_model_renderer(player_model_renderer);*/

	//sprite
	sprite = std::make_unique<Sprite>(device, L"Data/Sprite/earthmap.jpg");
	sprite->set_shader(sprite_shader);

	// static
	//ground_mesh = std::make_shared<StaticMesh>(device, "Data/Stage/map.fbx", false);
	//ground_object = std::make_shared<StaticObject>(ground_mesh);

	//ground_collision_mesh = std::make_shared<StaticMesh>(device, "Data/Stage/wall.fbx", false);
	//ground_collision_object = std::make_shared<StaticObject>(ground_collision_mesh);

	//// collision
	//ground_collision = std::make_shared<Collision>();
	//ground_collision->register_terrain(ground_collision_object);

	//マルチスレッド
	now_loading = true;
	loading_thread = std::make_unique<std::thread>(loading_thread_function, this, device);
}


void TitleScene::uninitialize()
{
	if (loading_thread && loading_thread->joinable()) loading_thread->join();
}
