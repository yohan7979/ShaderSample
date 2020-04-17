#pragma once
#include "ShaderClass.h"

class ReflectionShaderClass
	: public ShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct ReflectionBufferType
	{
		D3DXMATRIX reflection;
	};

	DECLARE_DERIVED_SHADER_CLASS(ReflectionShaderClass)

public:
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix, float alphaBlend = 1.f);

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix, float alphaBlend = 1.f);

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_reflectionBuffer;
	ID3D11SamplerState* m_sampleState;
};

