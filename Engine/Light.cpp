#include "pch.h"
#include "Light.h"
#include "Camera.h"
#include "Material.h"

Light::Light() : Component(ComponentType::Light)
{
	_shadowCamera = make_shared<GameObject>();
	_shadowCamera->AddComponent(make_shared<Transform>());
	_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음

}

Light::~Light()
{

}

void Light::Update()
{
	//RENDER->PushLightData(_desc);
}

void Light::FinalUpdate()
{
	_lightInfo.position = GetTransform()->GetPosition();

	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	_shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
	_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

	_shadowCamera->FinalUpdate();
}


void Light::Render()
{

	auto world = GetTransform()->GetWorldMatrix();
	_lightMaterial->GetShader()->PushTransformData(TransformDesc(world));

	if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = RESOURCES->Get<Texture>(L"ShadowTarget");
		_lightMaterial->SetNormalMap(shadowTex);

		Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
		_lightMaterial->GetShader()->GetMatrix("VP")->SetMatrix(reinterpret_cast<float *>(&matVP));
	}
	else
	{
		float scale = 2 * _lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	_lightMaterial->GetShader()->GetScalar("lightIndex")->SetInt(_lightIndex);
	_lightMaterial->Update();
	_volumeMesh->Update();

	// TODO lightIndex에 집어넣기.
	_lightMaterial->GetShader()->DrawIndexed(0, 0, _volumeMesh->GetIndexBuffer()->GetCount());
}

void Light::RenderShadow()
{
	_shadowCamera->GetCamera()->SortShadowObject();
	_shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightType(LIGHT_TYPE type)
{
	_lightInfo.lightType = static_cast<uint32>(type);


	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		_volumeMesh = RESOURCES->Get<Mesh>(L"Rectangle");
		_lightMaterial = RESOURCES->Get<Material>(L"DirLight");

		_shadowCamera->GetCamera()->SetFar(10000.f);
		_shadowCamera->GetCamera()->SetWidth(4096);
		_shadowCamera->GetCamera()->SetHeight(4096);

		break;
	case LIGHT_TYPE::POINT_LIGHT:
		_volumeMesh = RESOURCES->Get<Mesh>(L"Sphere");
		_lightMaterial = RESOURCES->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		_volumeMesh = RESOURCES->Get<Mesh>(L"Sphere");
		_lightMaterial = RESOURCES->Get<Material>(L"PointLight");
		break;
	}
}
