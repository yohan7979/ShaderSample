#include "LightClass.h"



LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass & other)
{
}


LightClass::~LightClass()
{
}

void LightClass::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = D3DXVECTOR4(r, g, b, a);
}

void LightClass::SetAmbientColor(float r, float g, float b, float a)
{
	m_ambientColor = D3DXVECTOR4(r, g, b, a);
}

void LightClass::SetSpecularColor(float r, float g, float b, float a)
{
	m_specularColor = D3DXVECTOR4(r, g, b, a);
}



void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

D3DXVECTOR4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

float LightClass::GetSpecularPower()
{
	return m_specularPower;
}

D3DXVECTOR3 LightClass::GetPosition()
{
	return m_position;
}

D3DXVECTOR4 LightClass::GetPositionVector4()
{
	return D3DXVECTOR4(m_position.x, m_position.y, m_position.z, 1.f);
}

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = D3DXVECTOR3(x, y, z);
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR3(x, y, z);
}

void LightClass::GenerateViewMatrix()
{
	D3DXVECTOR3 up;

	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;

	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_lookAt, &up);
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fov, screenAspect;

	fov = (float)D3DX_PI / 2.f;
	screenAspect = 1.f;

	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fov, screenAspect, screenNear, screenDepth);
}

void LightClass::GetViewMatrix(D3DXMATRIX & viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void LightClass::GetProjectionMatrix(D3DXMATRIX & projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void LightClass::GenerateOrthoMatrix(float width, float depthPlane, float nearPlane)
{
	D3DXMatrixOrthoLH(&m_orthoMatrix, width, width, nearPlane, depthPlane);
}

void LightClass::GetOrthoMatrix(D3DXMATRIX & orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void LightClass::Initialize(const D3DXVECTOR4& diffuse)
{
	SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	SetDiffuseColor(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpecularPower(32.f);
}

