#include "ObjectManager.h"
#include "ModelClass.h"
#include "D3DClass.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::ObjectManager(const ObjectManager & other)
{
}

ObjectManager::~ObjectManager()
{
	Shutdown();
}

bool ObjectManager::Initialize(D3DClass* D3D)
{
	if (D3D == nullptr)
	{
		return false;
	}

	bool result = true;

	result &= CreateObject(D3D->GetDevice(), "../MyEngine/Resource/Sphere.txt", L"../MyEngine/Resource/aa.bmp");
	result &= CreateObject(D3D->GetDevice(), "../MyEngine/Resource/Sphere.txt", L"../MyEngine/Resource/aa.bmp");
	result &= CreateObject(D3D->GetDevice(), "../MyEngine/Resource/plane01.txt", L"../MyEngine/Resource/wall01.dds");

	return result;
}

void ObjectManager::Shutdown()
{
	for (auto object : m_vecObjects)
	{
		if (object != nullptr)
		{
			delete object;
			object = nullptr;
		}
	}

	m_vecObjects.clear();
}

void ObjectManager::Render(struct ID3D11DeviceContext* deviceContext)
{
	for (auto object : m_vecObjects)
	{
		if (object)
		{
			object->Render(deviceContext);
		}
	}
}

bool ObjectManager::CreateObject(struct ID3D11Device* device, char* modelFilename, const WCHAR* textureFilename)
{
	// 모델 클래스 생성
	ModelClass* NewObject = new ModelClass;
	if (!NewObject)
	{
		return false;
	}
// 	// 모델 클래스 초기화
// 	if (!NewObject->Initialize(device, modelFilename, textureFilename))
// 	{
// 		return false;
// 	}

	m_vecObjects.push_back(NewObject);

	return true;
}
