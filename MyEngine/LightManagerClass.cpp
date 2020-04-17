#include "LightManagerClass.h"



LightManagerClass::LightManagerClass()
{
}

LightManagerClass::LightManagerClass(const LightManagerClass & other)
{
}


LightManagerClass::~LightManagerClass()
{
	Shutdown();
}

bool LightManagerClass::Initialize()
{
	LightClass* pLight = new DirectionalLightClass;
	pLight->Initialize(D3DXVECTOR4(1.f, 1.f, 1.f, 1.f));
	m_mapLight.insert(make_pair("DirectionalLight", pLight));

	pLight = new PointLightClass;
	pLight->Initialize(D3DXVECTOR4(1.f, 0.f, 0.f, 1.f));
	pLight->SetPosition(-3.f, 5.f, 3.f);
	m_mapLight.insert(make_pair("PointLight1", pLight));

	pLight = new PointLightClass;
	pLight->Initialize(D3DXVECTOR4(0.f, 1.f, 0.f, 1.f));
	pLight->SetPosition(3.f, 5.f, 3.f);
	m_mapLight.insert(make_pair("PointLight2", pLight));

	pLight = new PointLightClass;
	pLight->Initialize(D3DXVECTOR4(0.f, 0.f, 1.f, 1.f));
	pLight->SetPosition(-3.f, 5.f, -3.f);
	m_mapLight.insert(make_pair("PointLight3", pLight));

	pLight = new PointLightClass;
	pLight->Initialize(D3DXVECTOR4(1.f, 1.f, 1.f, 1.f));
	pLight->SetPosition(3.f, 5.f, -3.f);
	m_mapLight.insert(make_pair("PointLight4", pLight));

	return true;
}

void LightManagerClass::Shutdown()
{
	unordered_map<string, LightClass*>::iterator iter;
	unordered_map<string, LightClass*>::iterator iterEnd = m_mapLight.end();

	for (iter = m_mapLight.begin(); iter != iterEnd; ++iter)
	{
		delete iter->second;
		iter->second = 0;
	}
	m_mapLight.clear();

}

LightClass * LightManagerClass::GetLight(const string & LightName)
{
	unordered_map<string, LightClass*>::iterator iter = m_mapLight.find(LightName);
	if (iter == m_mapLight.end())
		return NULL;

	return iter->second;
}
