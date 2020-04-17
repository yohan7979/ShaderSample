#pragma once
#include <d3dx10math.h>
#include "Types.h"

struct Frustrum
{
	D3DXPLANE plane[6];
	D3DXVECTOR3 vertex[8];
	D3DXVECTOR3 center;

	bool IsInSphere(const D3DXVECTOR3& InPosition, float InRadius)
	{
		for (int i = 0; i < 6; ++i)
		{
			// 내적의 결과과 Radius 보다 크면 미포함
			if (D3DXPlaneDotCoord(&plane[i], &InPosition) > InRadius)
			{
				return false;
			}
		}

		return true;
	}
};

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass& other);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void RenderReflection(float height);
	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);
	void GetReflectionViewMatrix(D3DXMATRIX& reflectionMatrix);
	Frustrum GetFrustrum() const { return m_Frustrum; }

	bool CalculateMatrix(int screenWidth, int screenHeight, float screenNear, float screenDepth);
	void CalculateFrustrum();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;
	D3DXMATRIX m_reflectionViewMatrix;

	Frustrum m_Frustrum;
};

