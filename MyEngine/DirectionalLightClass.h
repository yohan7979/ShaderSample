#pragma once
#include "LightClass.h"

class DirectionalLightClass	:
	public LightClass
{
public:
	DirectionalLightClass();
	DirectionalLightClass(const DirectionalLightClass& other);
	~DirectionalLightClass();

public:
	virtual void Initialize(const D3DXVECTOR4& diffuse);
	void SetDirection(float x, float y, float z);
	D3DXVECTOR3 GetDirection();

private:
	D3DXVECTOR3 m_direction;
};

