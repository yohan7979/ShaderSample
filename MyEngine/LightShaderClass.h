#pragma once
#include "ShaderClass.h"
#include "Types.h"

class LightShaderClass
	: public ShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 specularColor;
		D3DXVECTOR3 lightDirection;
		float specularPower;  
	};

	struct PointLightPosBufferType
	{
		D3DXVECTOR4 pointlightPos[NUM_LIGHTS];

	};
	struct PointLightColorBufferType
	{
		D3DXVECTOR4 pointlightColor[NUM_LIGHTS];
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	DECLARE_DERIVED_SHADER_CLASS(LightShaderClass)

public:
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4,
		D3DXVECTOR4 specularColor, float specularPower, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[]);

private:
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4,
		D3DXVECTOR4 specularColor, float specularPower, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[]);

private:
	ID3D11SamplerState* m_sampleState;
	
	// 상수 버퍼
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_pointlightColorBuffer;
	ID3D11Buffer* m_pointlightPosBuffer;
};

