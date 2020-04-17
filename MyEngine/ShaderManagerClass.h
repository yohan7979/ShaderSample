#pragma once
#include "D3DClass.h"
#include "ShaderClass.h"
#include "ColorShaderClass.h"
#include "DepthShaderClass.h"
#include "FontShaderClass.h"
#include "LightShaderClass.h"
#include "ShadowShaderClass.h"
#include "TerrainShaderClass.h"
#include "TextureShaderClass.h"
#include "BumpMapShaderClass.h"
#include "ReflectionShaderClass.h"

#include <unordered_map>
using namespace std;

class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass& other);
	~ShaderManagerClass();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();

public:
	ShaderClass* GetShader(const string& shaderName);

	template<class T>
	T* GetShader(const string& shaderName)
	{
		ShaderClass* Shader = GetShader(shaderName);
		if (Shader == nullptr)
		{
			return nullptr;
		}

		return static_cast<T*>(Shader);
	}

private:
	unordered_map<string, ShaderClass*> m_mapShader;

};

