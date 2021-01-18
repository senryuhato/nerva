#include "game_scene.h"

#include "../NervaLib/key_input.h"

#include "imgui.h"
#include "framework.h"


void GameScene::loading_thread_function(GameScene* scene, ID3D11Device* device)
{
	std::lock_guard<std::mutex> lock(scene->loading_mutex);

	scene->now_loading = false;
}

void GameScene::initialize()
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
	player_model_renderer = std::make_shared<ModelRenderer>(device);
	player_model_renderer->set_shader(lambert_shader);

	player_model_resource = std::make_shared<ModelResource>(device, "Data/Player/player.fbx");

	player_model = std::make_shared<Model>(player_model_resource);
	player_model->play_animation(0, true);

	player_object = std::make_unique<PlayerObject>();
	player_object->initialize();
	player_object->transform.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	player_object->set_model(player_model);
	player_object->set_model_renderer(player_model_renderer);

	//sprite
	sprite = std::make_unique<Sprite>(device, L"Data/Sprite/earthmap.jpg");
	sprite->set_shader(sprite_shader);

	//マルチスレッド
	now_loading = true;
	loading_thread = std::make_unique<std::thread>(loading_thread_function, this, device);
}

void GameScene::update()
{
	if (is_now_loading()) return;

	//カメラ
	camera->update();

	player_model->update_animation(1.0f / 60.0f);

	//プレイヤー
	player_object->setCameraFrontRight(camera->get_front(), camera->get_right());
	player_object->update();
}

void GameScene::render()
{
	ID3D11DeviceContext* immediate_context = Framework::instance().get_immediate_context();

	//state
	blend_state->activate(immediate_context);

	if (is_now_loading()) return;

	DirectX::XMFLOAT4X4 view_projection;

	DirectX::XMStoreFloat4x4(&view_projection,
		DirectX::XMLoadFloat4x4(&camera->get_view()) * DirectX::XMLoadFloat4x4(&camera->get_projection()));

	DirectX::XMFLOAT4 light_direction(0, -1, 1, 0);

	//描画準備
	frame_buffer->clear(immediate_context, 1, 0, 0, 1);
	frame_buffer->activate(immediate_context);

	//描画
	player_object->render(immediate_context, view_projection, light_direction);
	sprite->render(immediate_context, { 0,0 }, { 1,1 }, 0, { 200,200 }, { 100,100 }, {100,100});

	//描画終了
	frame_buffer->deactivate(immediate_context);
	fullscreen_quad->blit(immediate_context, frame_buffer->render_target_shader_resource_view.Get(), true, true, true);

	//state
	blend_state->deactivate(immediate_context);
}

void GameScene::uninitialize()
{
	if (loading_thread && loading_thread->joinable()) loading_thread->join();
}