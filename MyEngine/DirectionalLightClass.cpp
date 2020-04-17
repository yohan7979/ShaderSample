#include "DirectionalLightClass.h"



DirectionalLightClass::DirectionalLightClass()
{
}

DirectionalLightClass::DirectionalLightClass(const DirectionalLightClass & other)
{
}

DirectionalLightClass::~DirectionalLightClass()
{
}

void DirectionalLightClass::Initialize(const D3DXVECTOR4& diffuse)
{
	LightClass::Initialize(diffuse);
	SetDirection(D3DX_PI / 3.f, 0.0f, 0.f);
	SetLookAt(0.f, 0.f, 0.f);
	GenerateOrthoMatrix(20.f, SHADOW_MAP_DEPTH, SHADOW_MAP_NEAR);
}

void DirectionalLightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR3 DirectionalLightClass::GetDirection()
{
	return m_direction;
}
