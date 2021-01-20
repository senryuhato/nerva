#include "static_object.h"

StaticObject::StaticObject(std::shared_ptr<StaticMesh>& mesh)
{
    this->mesh = mesh;
}

StaticObject::~StaticObject()
{
    mesh.reset();
}

void StaticObject::update()
{
    transform.calculate_world_transform_quaternion();
}
void StaticObject::render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& view_projection,
    const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color)
{
    DirectX::XMFLOAT4X4 wvp = {};
    DirectX::XMMATRIX W, VP, WVP;
    W = DirectX::XMLoadFloat4x4(&transform.world);
    VP = DirectX::XMLoadFloat4x4(&view_projection);
    WVP = W * VP;
    DirectX::XMStoreFloat4x4(&wvp, WVP);
    mesh->render(immediate_context, wvp, transform.world, light_direction, material_color);
}

int StaticObject::ray_pick(const DirectX::XMFLOAT3& start_position,
    const DirectX::XMFLOAT3& end_position,
    DirectX::XMFLOAT3* out_position,
    DirectX::XMFLOAT3* out_normal,
    float range)
{
    DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&transform.world);
    DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldTransform);

    //オブジェクト空間でのレイに変換
    DirectX::XMVECTOR worldStart = DirectX::XMLoadFloat3(&start_position);
    DirectX::XMVECTOR worldEnd = DirectX::XMLoadFloat3(&end_position);
    DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldStart, inverseTransform);
    DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(worldEnd, inverseTransform);

    //レイピック
    float outDistance;
    DirectX::XMFLOAT3 start, end;
    DirectX::XMStoreFloat3(&start, localStart);
    DirectX::XMStoreFloat3(&end, localEnd);
    int ret = mesh->ray_pick(start, end, out_position, out_normal, &outDistance, range);

    if (ret != -1)
    {
        //オブジェクト空間からワールド空間へ変換
        DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(out_position);
        DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(out_normal);
        DirectX::XMVECTOR worldPosition = DirectX::XMVector3TransformCoord(localPosition, worldTransform);
        DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, worldTransform);


        DirectX::XMStoreFloat3(out_position, worldPosition);
        DirectX::XMStoreFloat3(out_normal, worldNormal);
    }

    return ret;
}