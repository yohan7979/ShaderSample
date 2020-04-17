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

		// ���� �ε��Ѵ�.
		result = LoadModel(modelFilename);
		if (!result)
		{
			return false;
		}

		// �븻 ���
		CalculateModelVectors();

		// �ν��Ͻ̿� �ʿ��� ������ ĳ��
		CacheInstanceVector(device, instancedVector);

		// ����, �ε��� ���۸� �ʱ�ȭ�Ѵ�.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		// ���� ���� ���ø����� ��� �ؽ�ó �ε尡 ��������,
		// ���̴����� ���⽱�� ID3D11ShaderResourceView** Ÿ������ ��ȯ�Ѵ�.
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


	// ���� ���� ���ø����� ��� ����ŭ �ؽ�ó �ε�
	template<typename T, typename... Types>
	bool Initialize(ID3D11Device* device, char* modelFilename, const T* FirstArg, Types... SecondArgs)
	{
		// �ؽ�ó�� �ε��Ѵ�.
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
		// �ؽ�ó ����
		TextureClass* NewTexture = new TextureClass;
		if (!NewTexture)
		{
			return false;
		}

		// �ؽ�ó �ʱ�ȭ
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

