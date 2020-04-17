#include "TerrainClass.h"



TerrainClass::TerrainClass()	:
	m_vertexBuffer(0),
	m_indexBuffer(0),
	m_heightMap(0),
	m_Texture(0)
{
}

TerrainClass::TerrainClass(const TerrainClass & other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::Initialize(ID3D11Device * device, char* heightMapFilename, WCHAR* textureFilename)
{
	bool result;

	// HeightMap 로딩하고 정규화
	result = LoadHeightMap(heightMapFilename);
	if (!result)
	{
		return false;
	}
	NormalizeHeightMap();

	// 노말 계산하기
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// 텍스처 좌표 계산
	CalculateTextureCoordinates();

	// 텍스처 로딩
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::Shutdown()
{
	ShutdownBuffers();
	ShutdownHeightMap();
	ReleaseTexture();
}

void TerrainClass::Render(ID3D11DeviceContext * deviceContext)
{
	RenderBuffers(deviceContext);
}

bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

bool TerrainClass::LoadHeightMap(char * filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	
	// 파일 열기
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	BITMAPFILEHEADER bitmapFileHeader;
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	BITMAPINFOHEADER bitmapInfoHeader;
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	int imageSize = m_terrainWidth * m_terrainHeight * 3;
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일 닫기
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// 비트맵에서 얻어온 하이트맵 크기만큼 동적할당을 해준다.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// 비트맵으로부터 높이 정보를 읽어온다.
	int k = 0;
	int index;
	unsigned char height;

	for (int i = 0; i < m_terrainHeight; ++i)
	{
		for (int j = 0; j < m_terrainWidth; ++j)
		{
			height = bitmapImage[k];
			index = (m_terrainHeight * i) + j;

			m_heightMap[index].x = (float)j;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)i;
			
			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void TerrainClass::NormalizeHeightMap()
{
	for (int i = 0; i < m_terrainHeight; ++i)
	{
		for (int j = 0; j < m_terrainWidth; ++j)
		{
			m_heightMap[(m_terrainHeight * i) + j].y /= 15.f;
		}
	}
}

void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}

void TerrainClass::CalculateTextureCoordinates()
{
	int incrementCount, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;

	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	tuCoordinate = 0.f;
	tvCoordinate = 1.f;
	tuCount = tvCount = 0;

	for (int i = 0; i < m_terrainHeight; ++i)
	{
		for (int j = 0; j < m_terrainWidth; ++j)
		{
			m_heightMap[(m_terrainHeight * i) + j].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * i) + j].tv = tvCoordinate;

			tuCoordinate += incrementValue;
			tuCount++;

			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.f;
				tuCount = 0;
			}
		}

		tvCoordinate -= incrementValue;
		tvCount++;

		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.f;
			tvCount = 0;
		}
	}
}

bool TerrainClass::LoadTexture(ID3D11Device * device, WCHAR * filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}

bool TerrainClass::InitializeBuffers(ID3D11Device * device)
{
	VertexType* vertices;
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	unsigned long* indices;
	m_indexCount = m_vertexCount;

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}


	int index = 0;
	int index1, index2, index3, index4;
	float tu, tv;

	// Load the vertex and index array with the terrain data.
	for (int j = 0; j< (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i< (m_terrainWidth - 1); i++)
		{
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
			index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.

			 // Upper left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index3].tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { tu = 1.0f; }

			vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			vertices[index].texture = D3DXVECTOR2(tu, m_heightMap[index2].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	CreateVertexBuffer(device, vertices, m_vertexCount);
	CreateIndexBuffer(device, indices, m_indexCount);

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

bool TerrainClass::CreateVertexBuffer(ID3D11Device * device, void * vertices, int vertexCount)
{
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

bool TerrainClass::CreateIndexBuffer(ID3D11Device * device, unsigned long * indices, int indexCount)
{
	HRESULT result;
	D3D11_BUFFER_DESC IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA IndexData;

	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	IndexData.pSysMem = indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&IndexBufferDesc, &IndexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void TerrainClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void TerrainClass::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
