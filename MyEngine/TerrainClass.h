#pragma once
#include <d3d11.h>
#include <D3DX10math.h>
#include <stdio.h>
#include "TextureClass.h"

const int TEXTURE_REPEAT = 16;

class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass& other);
	~TerrainClass();

	bool Initialize(ID3D11Device* device, char* heightMapFilename, WCHAR* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount() { return m_indexCount; }
	ID3D11ShaderResourceView* GetTexture() { return m_Texture->GetTexture(); }

	bool CalculateNormals();
	bool LoadHeightMap(char* filename);
	void NormalizeHeightMap();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void ReleaseTexture();

private:
	bool InitializeBuffers(ID3D11Device* device);
	bool CreateVertexBuffer(ID3D11Device* device, void* vertices, int vertexCount);
	bool CreateIndexBuffer(ID3D11Device* device, unsigned long* indices, int indexCount);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;

	HeightMapType* m_heightMap;
	TextureClass* m_Texture;
};

