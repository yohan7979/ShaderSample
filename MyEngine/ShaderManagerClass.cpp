#include "ShaderManagerClass.h"

ShaderManagerClass::ShaderManagerClass()
{
}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass & other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
	Shutdown();
}

bool ShaderManagerClass::Initialize(ID3D11Device * device, HWND hwnd)
{
	ShaderClass* pShader = new ColorShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("ColorShader", pShader));

	pShader = new DepthShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("DepthShader", pShader));

	pShader = new FontShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("FontShader", pShader));

	pShader = new LightShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("LightShader", pShader));

	pShader = new ShadowShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("ShadowShader", pShader));

	pShader = new TerrainShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("TerrainShader", pShader));

	pShader = new TextureShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("TextureShader", pShader));

	pShader = new BumpMapShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("BumpMapShader", pShader));

	pShader = new ReflectionShaderClass;
	pShader->Initialize(device, hwnd);
	m_mapShader.insert(make_pair("ReflectionShader", pShader));

	return true;
}

void ShaderManagerClass::Shutdown()
{
	unordered_map<string, ShaderClass*>::iterator iter;
	unordered_map<string, ShaderClass*>::iterator iterEnd = m_mapShader.end();
	for (iter = m_mapShader.begin(); iter != iterEnd; ++iter)
	{
		iter->second->Shutdown();
	}

	m_mapShader.clear();
}

ShaderClass * ShaderManagerClass::GetShader(const string & shaderName)
{
	unordered_map<string, ShaderClass*>::iterator iter = m_mapShader.find(shaderName);
	
	if (iter == m_mapShader.end())
		return NULL;

	return iter->second;
}
