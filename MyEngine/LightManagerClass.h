#pragma once
#include "LightClass.h"
#include "DirectionalLightClass.h"
#include "PointLightClass.h"

#include <unordered_map>
using namespace std;

class LightManagerClass
{
public:
	LightManagerClass();
	LightManagerClass(const LightManagerClass& other);
	~LightManagerClass();

public:
	bool Initialize();
	void Shutdown();

public:
	LightClass* GetLight(const string& LightName);

private:
	unordered_map<string, LightClass*> m_mapLight;

};

