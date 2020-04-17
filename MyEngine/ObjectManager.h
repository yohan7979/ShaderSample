#pragma once
#include <vector>
#include <windows.h>
using namespace std;

class ModelClass;
class D3DClass;

class ObjectManager
{
public:
	ObjectManager();
	ObjectManager(const ObjectManager& other);
	virtual ~ObjectManager();

public:
	bool Initialize(D3DClass* D3D);
	void Shutdown();
	void Render(struct ID3D11DeviceContext* deviceContext);

	bool CreateObject(struct ID3D11Device* device, char* modelFilename, const WCHAR* textureFilename);

private:
	vector<class ModelClass*> m_vecObjects;
};

