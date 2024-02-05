#pragma once
#include "Component.h"


enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();
	
	virtual void Update();
	virtual void FinalUpdate();
	void Render();
	void RenderShadow();

public:
	LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); }

	const LightInfo& GetLightInfo() { return _lightInfo; }

	LightDesc& GetLightDesc() { return _desc; }

	void SetLightDesc(LightDesc& desc) { _desc = desc; }
	void SetAmbient(const Color& color) { _desc.ambient = color; }
	void SetDiffuse(const Color& color) { _desc.diffuse = color; }
	void SetSpecular(const Color& color) { _desc.specular = color; }
	void SetEmissive(const Color& color) { _desc.emissive = color; }
	void SetLightDirection(Vec3 direction) { _desc.direction = direction; }

	void SetLightType(LIGHT_TYPE type);
private:
	LightInfo _lightInfo = {};

	LightDesc _desc;
	shared_ptr<GameObject> _shadowCamera;


	int8 _lightIndex = -1;
};

