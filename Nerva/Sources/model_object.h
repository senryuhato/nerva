#pragma once

#include "model_renderer.h"
#include "component.h"

#include "../NervaLib/shader.h"

class ModelObject
{
public:
	std::shared_ptr<ModelRenderer> model_renderer;
	std::shared_ptr<Model> model;
	Transform transform;
	void set_model_renderer(std::shared_ptr<ModelRenderer>& model_renderer)  { this->model_renderer = model_renderer; }
	void set_model(std::shared_ptr<Model>& model) { this->model = model; }
	virtual void initialize();
	virtual void update();
	virtual void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction);
};