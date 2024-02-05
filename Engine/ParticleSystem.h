#pragma once
#include "Component.h"



class ParticleSystem : public Component
{
	using Super = Component;
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void Update();
	void Render();
};

