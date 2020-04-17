#pragma once
#include "ShaderClass.h"

class DepthShaderClass 
	: public ShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	DECLARE_DERIVED_SHADER_CLASS(DepthShaderClass)

public:
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	ID3D11Buffer* m_matrixBuffer;
};

