#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <vector>
using namespace std;

#include "textureclass.h"


class ModelClass
{
protected:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass& other);
	~ModelClass();

	bool Initialize(ID3D11Device* device, char* modelFilename, const vector<D3DXVECTOR3>& instancedVector = vector<D3DXVECTOR3>())
	{
		bool result;

		// 모델을 로딩한다.
		result = LoadModel(modelFilename);
		if (!result)
		{
			return false;
		}

		// 노말 계산
		CalculateModelVectors();

		// 인스턴싱에 필요한 데이터 캐싱
		CacheInstanceVector(device, instancedVector);

		// 정점, 인덱스 버퍼를 초기화한다.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		// 가변 인자 템플릿으로 모든 텍스처 로드가 끝났으니,
		// 쉐이더에서 쓰기쉽게 ID3D11ShaderResourceView** 타입으로 변환한다.
		m_textureCount = static_cast<int>(m_vecTexture.size());
		m_SRVs = new ID3D11ShaderResourceView*[m_textureCount];

		for (int i = 0 ; i < m_textureCount; ++i)
		{
			if (m_vecTexture[i])
			{
				m_SRVs[i] = m_vecTexture[i]->GetTexture();
			}
		}

		return true;
	}


	// 가변 인자 템플릿으로 경로 수만큼 텍스처 로딩
	template<typename T, typename... Types>
	bool Initialize(ID3D11Device* device, char* modelFilename, const T* FirstArg, Types... SecondArgs)
	{
		// 텍스처를 로딩한다.
		if (!LoadTexture(device, FirstArg))
		{
			return false;
		}

		return Initialize(device, modelFilename, SecondArgs...);
	}

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount() { return m_vertexCount; }
	int GetIndexCount();
	int GetTextureCount() const { return m_textureCount; }
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextures();

protected:
	virtual bool InitializeBuffers(ID3D11Device* device);
	bool CreateVertexBuffer(ID3D11Device* device, void* vertices, int vertexCount);
	bool CreateIndexBuffer(ID3D11Device* device, unsigned long* indices, int indexCount);
	virtual void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext* deviceContext);
	virtual void CacheInstanceVector(ID3D11Device* device, const vector<D3DXVECTOR3>& instanceVector) {}

	template<typename T>
	bool LoadTexture(ID3D11Device* device, T* filename)
	{
		return false;
	}

	template<>
	bool LoadTexture(ID3D11Device* device, const WCHAR* filename)
	{
		// 텍스처 생성
		TextureClass* NewTexture = new TextureClass;
		if (!NewTexture)
		{
			return false;
		}

		// 텍스처 초기화
		if (!NewTexture->Initialize(device, filename))
		{
			return false;
		}

		m_vecTexture.push_back(NewTexture);
		return true;
	}
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(const ModelType& v1, const ModelType& v2, const ModelType& v3, D3DXVECTOR3& tangent, D3DXVECTOR3& binormal);
	void CalculateNormal(const D3DXVECTOR3& tangent, const D3DXVECTOR3& binormal, D3DXVECTOR3& normal);

protected:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	vector<TextureClass*> m_vecTexture;
	int m_textureCount;

	ID3D11ShaderResourceView** m_SRVs;
	ModelType* m_model;
};

