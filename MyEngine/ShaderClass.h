#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

#define DECLARE_SHADER_CLASS(ShaderClassType) \
public:\
ShaderClassType();\
ShaderClassType(const ShaderClassType& other);\
virtual ~ShaderClassType();\
virtual bool Initialize(ID3D11Device* device, HWND hwnd);\
virtual void Shutdown();\
protected:\
virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, const char* vsName, const char* psName);\
virtual void ShutdownShader();\
virtual void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

#define DECLARE_DERIVED_SHADER_CLASS(ShaderClassType) DECLARE_SHADER_CLASS(ShaderClassType)\
public:\
typedef ShaderClass Super;


class ShaderClass
{
	DECLARE_SHADER_CLASS(ShaderClass)

public:
	void OutputShaderErrorMessage(ID3D10Blob * m_errorMessage, HWND hwnd, WCHAR * shaderFilename);

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;

	ID3D10Blob* m_errorMessage;
	ID3D10Blob* m_vertexShaderBuffer;
	ID3D10Blob* m_pixelShaderBuffer;
};

