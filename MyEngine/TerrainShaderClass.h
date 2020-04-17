#pragma once
#include "ShaderClass.h"
#include "Types.h"

class TerrainShaderClass
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
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	struct PointLightPosBufferType
	{
		D3DXVECTOR4 pointlightPos[NUM_LIGHTS];

	};
	struct PointLightColorBufferType
	{
		D3DXVECTOR4 pointlightColor[NUM_LIGHTS];
	};

	DECLARE_DERIVED_SHADER_CLASS(TerrainShaderClass)

public:
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
		D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDir, ID3D11ShaderResourceView*, D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[]);

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
		D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDir, ID3D11ShaderResourceView*, D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[]);

	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_pointlightColorBuffer;
	ID3D11Buffer* m_pointlightPosBuffer;
};

