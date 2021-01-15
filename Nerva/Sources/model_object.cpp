#include "model_object.h"

void ModelObject::initialize()
{
	transform.initialize();
}
void ModelObject::update()
{
	transform.update();
	transform.calculate_world_transform_quaternion();
}
void ModelObject::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction)
{
	if (model)
	{
		model->calculate_local_transform();
		model->calculate_world_transform(DirectX::XMLoadFloat4x4(&transform.world));
		if (model_renderer)
		{
			model_renderer->begin(immediate_context, view_projection, light_direction);
			model_renderer->draw(immediate_context, model.get());
			model_renderer->end(immediate_context);
		}
	}
}
