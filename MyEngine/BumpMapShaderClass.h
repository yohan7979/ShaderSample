#pragma once
#include "ShaderClass.h"
class BumpMapShaderClass :
	public ShaderClass
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
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	DECLARE_DERIVED_SHADER_CLASS(BumpMapShaderClass)

public:
	bool Render(ID3D11DeviceContext* deviceContext, int vertexCount, int indexCount, int instanceCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textures, int textureCount, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor);

	void RenderShaderInstanced(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount);

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textures, int textureCount, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor);

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_LightBuffer;
	ID3D11SamplerState* m_sampleState;
};

