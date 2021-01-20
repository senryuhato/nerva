#include "debug_scene.h"

#include "../NervaLib/key_input.h"

#include "imgui.h"
#include "framework.h"


void DebugScene::loading_thread_function(DebugScene* scene, ID3D11Device* device)
{
	std::lock_guard<std::mutex> lock(scene->loading_mutex);

	scene->now_loading = false;
}

void DebugScene::initialize()
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

	player_model_resource = std::make_shared<ModelResource>(device, "Data/player/player.fbx");

	player_model = std::make_shared<Model>(player_model_resource);
	player_model->play_animation(0, true);

	player_object = std::make_unique<PlayerObject>();
	player_object->initialize();
	player_object->transform.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	player_object->set_model(player_model);
	player_object->set_model_renderer(player_model_renderer);*/

	//boss
	boss_model_renderer = std::make_shared<ModelRenderer>(device, false);
	boss_model_renderer->set_shader(lambert_shader);

	boss_model_resource = std::make_shared<ModelResource>(device, "Data/Boss/team_Doragon.fbx");
	//boss_model_resource = std::make_shared<ModelResource>(device, "Data/Player/EasyTank_Separate.fbx");
	boss_model = std::make_shared<Model>(boss_model_resource);
	boss_model->play_animation(0, true);

	boss_object = std::make_unique<BossObject>();
	boss_object->initialize();
	//boss_object->transform.scale = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
	boss_object->set_model(boss_model);
	boss_object->set_model_renderer(boss_model_renderer);

	//sprite
	/*sprite = std::make_unique<Sprite>(device, L"Data/Sprite/earthmap.jpg");
	sprite->set_shader(sprite_shader);*/

	//static
	//ground_mesh = std::make_shared<StaticMesh>(device, "Data/Stage/stage01.fbx", false);
	ground_mesh = std::make_shared<StaticMesh>(device, "Data/Stage/background.fbx", false);
	ground_object = std::make_shared<StaticObject>(ground_mesh);

	//collision
	ground_collision = std::make_shared<Collision>(); 
	ground_collision->register_terrain(ground_object);

	//マルチスレッド
	now_loading = true;
	loading_thread = std::make_unique<std::thread>(loading_thread_function, this, device);
}

void DebugScene::update()
{
	if (is_now_loading()) return;

	//カメラ
	camera->update();

	
	//boss
	boss_model->update_animation(1.0f / 60.0f);
	boss_object->update(ground_collision);

	//プレイヤー
	//player_model->update_animation(1.0f / 60.0f);
	//player_object->update();

	if (KeyInput::key_trigger() & KEY_ENTER) SceneManager::instance().change_scene("GAME");
	if (KeyInput::key_trigger() & KEY_Q) SceneManager::instance().change_scene("DEBUG");
}

void DebugScene::render()
{
	ID3D11DeviceContext* immediate_context = Framework::instance().get_immediate_context();

	//state
	blend_state->activate(immediate_context);

	if (is_now_loading()) return;

	DirectX::XMFLOAT4X4 view_projection;

	DirectX::XMMATRIX C = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	DirectX::XMStoreFloat4x4(&view_projection,
		C * DirectX::XMLoadFloat4x4(&camera->get_view()) * DirectX::XMLoadFloat4x4(&camera->get_projection()));

	

	DirectX::XMFLOAT4 light_direction(0, -1, 1, 0);

	//描画準備
	frame_buffer->clear(immediate_context, 1, 0, 0, 1);
	frame_buffer->activate(immediate_context);

	//描画
	ground_object->render(immediate_context, view_projection, light_direction, {1.0f, 0.0f,0.0f,1.0f});

	boss_object->render(immediate_context, view_projection, light_direction);
	//player_object->render(immediate_context, view_projection, light_direction);
	//sprite->render(immediate_context, { 0,0 }, { 1,1 }, 0, { 200,200 }, { 100,100 }, { 100,100 });

	//描画終了
	frame_buffer->deactivate(immediate_context);
	fullscreen_quad->blit(immediate_context, frame_buffer->render_target_shader_resource_view.Get(), true, true, true);

	//state
	blend_state->deactivate(immediate_context);
}

void DebugScene::uninitialize()
{
	if (loading_thread && loading_thread->joinable()) loading_thread->join();
}