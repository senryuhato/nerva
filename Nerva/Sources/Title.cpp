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


	//sprite
	sprite = std::make_unique<Sprite>(device, L"Data/Sprite/title.png");
	sprite->set_shader(sprite_shader);

	//マルチスレッド
	now_loading = true;
	loading_thread = std::make_unique<std::thread>(loading_thread_function, this, device);
}

void TitleScene::update()
{
	if (is_now_loading()) return;

	//カメラ
	// camera->set_rotate(DirectX::XMFLOAT3(player_object->rotate.x, player_object->rotate.y, player_object->rotate.z));
	//{
	//	float dist = 10.0f;
	//	DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(player_object->transform.position);
	//	// camera->set_eye(f);
	//	f.x = f.x - sinf(camera->get_rotate().y) * dist;
	//	f.y = f.y + cosf(0) * dist / 3;
	//	f.z = f.z - cosf(camera->get_rotate().y) * dist;
	//	camera->set_focus(f);
	//}
	camera->update();

	//float time = 60.0f;
	//if (player_model->current_animation == 0)time = 60.f;
	//if (player_model->current_animation == 1 || player_model->current_animation == 2 || player_model->current_animation == 3)time = 120.0f;
	//if (player_model->current_animation == 5)time = 180.0f;

	//player_model->update_animation(1.0f / time);

	////プレイヤー
	//player_object->setCameraFrontRight(camera->get_front(), camera->get_right());
	//player_object->setCameraAngle(camera->get_rotate());
	//player_object->update(ground_collision);



}

void TitleScene::render()
{
	ID3D11DeviceContext* immediate_context = Framework::instance().get_immediate_context();

	//state
	// blend_state->activate(immediate_context);

	if (is_now_loading()) return;

	DirectX::XMFLOAT4X4 view_projection;

	DirectX::XMStoreFloat4x4(&view_projection,
		DirectX::XMLoadFloat4x4(&camera->get_view()) * DirectX::XMLoadFloat4x4(&camera->get_projection()));

	DirectX::XMFLOAT4 light_direction(0, -1, 1, 0);

	//描画準備
	frame_buffer->clear(immediate_context, 1, 0, 0, 1);
	frame_buffer->activate(immediate_context);

	//描画
	/*ground_object->render(immediate_context, view_projection, light_direction);
	player_object->render(immediate_context, view_projection, light_direction);*/
	sprite->render(immediate_context, { 0,0 }, { 1,1 }, 0, { 1280,920 }, { 0,200 }, { 1920,1080 });

	//描画終了
	frame_buffer->deactivate(immediate_context);
	fullscreen_quad->blit(immediate_context, frame_buffer->render_target_shader_resource_view.Get(), true, true, true);

	//state
	// blend_state->deactivate(immediate_context);
}


void TitleScene::uninitialize()
{
	if (loading_thread && loading_thread->joinable()) loading_thread->join();
}
