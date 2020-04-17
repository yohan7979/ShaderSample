#pragma once
#include "LightClass.h"

class PointLightClass :
	public LightClass
{
public:
	PointLightClass();
	PointLightClass(const PointLightClass& other);
	~PointLightClass();

public:
	virtual void Initialize(const D3DXVECTOR4& diffuse);
};

