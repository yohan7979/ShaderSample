#include "ModelClass.h"



ModelClass::ModelClass()	:
	m_vertexBuffer(0),
	m_indexBuffer(0),
	m_model(0),
	m_SRVs(nullptr)
{
}

ModelClass::ModelClass(const ModelClass & other)
{
}


ModelClass::~ModelClass()
{
}

void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// ���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �غ��Ų��.
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_SRVs[0] ? m_SRVs[0] : nullptr;
}

ID3D11ShaderResourceView** ModelClass::GetTextures()
{
	return m_SRVs;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� ���� ����
	 VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (int i = 0; i< m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);
	}

	CreateVertexBuffer(device, vertices, m_vertexCount);

	delete[] vertices;
	vertices = 0;


	// �ε��� ���� ����
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the index array with data.
	for (int i = 0; i< m_indexCount; i++)
	{
		indices[i] = i;
	}

	CreateIndexBuffer(device, indices, m_indexCount);

	delete[] indices;
	indices = 0;


	return true;
}

bool ModelClass::CreateVertexBuffer(ID3D11Device * device, void* vertices, int vertexCount)
{
	HRESULT result;
	
	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

bool ModelClass::CreateIndexBuffer(ID3D11Device * device, unsigned long* indices, int indexCount)
{
	HRESULT result;

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void ModelClass::ShutdownBuffers()
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

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// �Է� �����⿡ ���� ���۸� ����
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �Է� �����⿡ �ε��� ���۸� ����
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ModelClass::ReleaseTexture()
{
	for (auto texture : m_vecTexture)
	{
		if (texture)
		{
			texture->Shutdown();
			delete texture;
			texture = nullptr;
		}
	}

	delete[] m_SRVs;

	m_vecTexture.clear();
}

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	// ���� ���� ������ ���� �ؽ�Ʈ ������ ����.
	fin.open(filename);

	// ������ ������ ������ false
	if (fin.fail())
	{
		return false;
	}

	// ���� ������ �д´�.
	input = 0;
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;

	// ���� ������ ������ ���� �����Ѵ�.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	input = 0;
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���� ������ �о� �𵨿� �����Ѵ�.
	for (i = 0; i < m_vertexCount; ++i)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}

void ModelClass::CalculateModelVectors()
{
	ModelType v1, v2, v3;
	D3DXVECTOR3 tangent, binormal, normal;

	int faceCount = m_vertexCount / 3;
	int index = 0;

	// �� �鿡 ���� ���
	for (int i = 0; i < faceCount; ++i)
	{
		// �� �鿡 ���� 3���� ���� ����
		v1 = m_model[index++];
		v2 = m_model[index++];
		v3 = m_model[index++];

		// ǥ���� ź��Ʈ�� ���̳븻 ���
		CalculateTangentBinormal(v1, v2, v3, tangent, binormal);

		// ź��Ʈ�� ���̳븻�� �̿��Ͽ� �븻 ����
		CalculateNormal(tangent, binormal, normal);

		// ���� ������ ���� n / t / b ����
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;
	}
}

void ModelClass::CalculateTangentBinormal(const ModelType& v1, const ModelType& v2, const ModelType& v3, D3DXVECTOR3& tangent, D3DXVECTOR3& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];

	vector1[0] = v2.x - v1.x;
	vector1[1] = v2.y - v1.y;
	vector1[2] = v2.z - v1.z;

	vector2[0] = v3.x - v1.x;
	vector2[1] = v3.y - v1.y;
	vector2[2] = v3.z - v1.z;

	tuVector[0] = v2.tu - v1.tu;
	tvVector[0] = v2.tv - v1.tv;

	tuVector[1] = v3.tu - v1.tu;
	tvVector[1] = v3.tv - v1.tv;

	float den = 1.f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
	tangent /= length;

	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
	binormal /= length;
}

void ModelClass::CalculateNormal(const D3DXVECTOR3& tangent, const D3DXVECTOR3& binormal, D3DXVECTOR3& normal)
{
	// ź��Ʈ�� ���̳븻�� �������� �븻�� ���ϰ�, ����ȭ
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
	normal /= length;
}
