#pragma once

#include <memory>
#include <DirectXMath.h>
#include "../NervaLib/static_mesh.h"

#include "component.h"

class StaticObject
{
public:
    Transform transform;
    std::shared_ptr<StaticMesh> mesh;
public:
    StaticObject(std::shared_ptr<StaticMesh>& mesh);
    ~StaticObject();
    void update();  //ÉèÅ[ÉãÉhçsóÒçÏê¨  
    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& view_projection,
        const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color = {1.0f,1.0f,1.0f,1.0f});

    int ray_pick(
        const DirectX::XMFLOAT3& start_position,
        const DirectX::XMFLOAT3& end_position,
        DirectX::XMFLOAT3* out_position,
        DirectX::XMFLOAT3* out_normal,
        float range = 0.0f);
};
