#pragma once
#include <d3dx10math.h>
#include "Types.h"

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass& other);
	virtual ~LightClass();

public:
	void SetDiffuseColor(float r, float g, float b, float a);
	void SetAmbientColor(float r, float g, float b, float a);
	void SetSpecularColor(float r, float g, float b, float a);
	void SetSpecularPower(float power);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR4 GetPositionVector4();
	void SetLookAt(float x, float y, float z);
	void SetPosition(float x, float y, float z);
	
public:
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);
	void GenerateOrthoMatrix(float width, float depthPlane, float nearPlane);
	
	void GetViewMatrix(D3DXMATRIX& viewMatrix);
	void GetProjectionMatrix(D3DXMATRIX& projectionMatrix);
	void GetOrthoMatrix(D3DXMATRIX& orthoMatrix);

public:
	virtual void Initialize(const D3DXVECTOR4& diffuse);

protected:
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_specularColor;
	float m_specularPower;

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;
};

