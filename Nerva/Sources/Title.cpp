#include "../NervaLib/key_input.h"
#include "framework.h"
#include "Title.h"

void TitleScene::loading_thread_function(TitleScene* scene, ID3D11Device* device)
{
	std::lock_guard<std::mutex> lock(scene->loading_mutex);

	//shader
	scene->lambert_shader = std::make_shared<LambertShader>(device);

	//プレイヤー
	scene->player_model_renderer = std::make_shared<ModelRenderer>(device);
	scene->player_model_renderer->set_shader(scene->lambert_shader);

	scene->player_model_resource = std::make_shared<ModelResource>(device, "Data/Player/001.fbx");

	scene->player_model = std::make_shared<Model>(scene->player_model_resource);
	scene->player_model->play_animation(0, true);

	scene->player_object = std::make_shared<PlayerObject>();
	scene->player_object->initialize();
	scene->player_object->transform.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	scene->player_object->set_model(scene->player_model);
	scene->player_object->set_model_renderer(scene->player_model_renderer);

	// static
	scene->ground_mesh = std::make_shared<StaticMesh>(device, "Data/Stage/map.fbx", false);
	scene->ground_object = std::make_shared<StaticObject>(scene->ground_mesh);

	scene->ground_collision_mesh = std::make_shared<StaticMesh>(device, "Data/Stage/wall.fbx", false);
	scene->ground_collision_object = std::make_shared<StaticObject>(scene->ground_collision_mesh);

	// collision
	scene->ground_collision = std::make_shared<Collision>();
	scene->ground_collision->register_terrain(scene->ground_collision_object);

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


	sprite_shader = std::make_shared<SpriteShader>(device);

	//sprite
	sprite = std::make_unique<Sprite>(device, L"Data/Sprite/title.png");
	sprite->set_shader(sprite_shader);
	start = std::make_unique<Sprite>(device, L"Data/Sprite/clickStart.png");
	start->set_shader(sprite_shader);
	nowString = std::make_unique<Sprite>(device, L"Data/UI/nowloading.png");
	nowString->set_shader(sprite_shader);
	nowWheel = std::make_unique<Sprite>(device, L"Data/UI/now.png");
	nowWheel->set_shader(sprite_shader);

	//マルチスレッド
	now_loading = true;
	loading_thread = std::make_unique<std::thread>(loading_thread_function, this, device);
}

void TitleScene::update()
{
	if (is_now_loading()) return;
	
	if (KeyInput::key_trigger() & KEY_LBUTTON) SceneManager::instance().change_scene("GAME");
	

	//カメラ
	{
		float dist = 10.0f;
		DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(player_object->transform.position);
		// camera->set_eye(f);
		f.x = f.x - sinf(camera->get_rotate().y) * dist;
		f.y = f.y + cosf(0) * dist / 3;
		f.z = f.z - cosf(camera->get_rotate().y) * dist;
		camera->set_focus(f);
	}
	camera->update();

	float time = 60.0f;
	// if (player_model->current_animation == 0)time = 60.f;
	// if (player_model->current_animation == 1 || player_model->current_animation == 2 || player_model->current_animation == 3)time = 120.0f;
	if (player_model->current_animation == 5)time = 180.0f;

	player_model->update_animation(1.0f / time);

	//プレイヤー
	player_object->setCameraFrontRight(camera->get_front(), camera->get_right());
	player_object->setCameraAngle(camera->get_rotate());
	player_object->update(ground_collision);



}

void TitleScene::render()
{
	ID3D11DeviceContext* immediate_context = Framework::instance().get_immediate_context();

	//state
	blend_state->activate(immediate_context);

	if (is_now_loading())
	{
		static float angle = 0.0f;
		nowString->render(immediate_context, { 0,300 }, { 0.5,0.5 }, angle, { 1920,1080 }, { 0,0 }, { 1920,1080 });
		nowWheel->render(immediate_context, { 100,-200 }, { 0.5,0.5 }, 0, { 1920,1080 }, { 0,0 }, { 1920,1080 });
		angle += 0.1;
		return;
	}

	DirectX::XMFLOAT4X4 view_projection;

	DirectX::XMStoreFloat4x4(&view_projection,
		DirectX::XMLoadFloat4x4(&camera->get_view()) * DirectX::XMLoadFloat4x4(&camera->get_projection()));

	DirectX::XMFLOAT4 light_direction(0, -1, 1, 0);

	//描画準備
	frame_buffer->clear(immediate_context, 1, 0, 0, 1);
	frame_buffer->activate(immediate_context);

	//描画
	ground_object->render(immediate_context, view_projection, light_direction);
	player_object->render(immediate_context, view_projection, light_direction);

	sprite->render(immediate_context, { 0,200 }, { 1,1 }, 0, { 1280,920 }, { 0,0 }, { 1920,1080 });
	start->render(immediate_context, { 400,-400 }, { 1,1 }, 0, { 500,200 }, { 0,0 }, { 500,200 });

	//描画終了
	frame_buffer->deactivate(immediate_context);
	fullscreen_quad->blit(immediate_context, frame_buffer->render_target_shader_resource_view.Get(), true, true, true);


	//state
	blend_state->deactivate(immediate_context);
}


void TitleScene::uninitialize()
{
	if (loading_thread && loading_thread->joinable()) loading_thread->join();
}
