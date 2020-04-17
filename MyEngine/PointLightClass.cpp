#include "PointLightClass.h"



PointLightClass::PointLightClass()
{
}

PointLightClass::PointLightClass(const PointLightClass& other)
{
}


PointLightClass::~PointLightClass()
{
}

void PointLightClass::Initialize(const D3DXVECTOR4 & diffuse)
{
	LightClass::Initialize(diffuse);
}

