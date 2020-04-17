#include "InstancedModelClass.h"

InstancedModelClass::InstancedModelClass() 
	: m_instanceBuffer(nullptr)
{

}

InstancedModelClass::~InstancedModelClass()
{
	m_vecInstances.clear();
}

bool InstancedModelClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 버퍼 생성
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (int i = 0; i < m_vertexCount; i++)
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


	// 인스턴스 버퍼 생성
	m_instanceCount = static_cast<int>(m_vecInstances.size());
	InstanceType* Instances = new InstanceType[m_instanceCount];
	if (!Instances)
	{
		return false;
	}

	for (int i = 0; i < m_instanceCount; ++i)
	{
		Instances[i].position = m_vecInstances[i];
	}

	CreateInstanceBuffer(device, Instances);

	delete[] Instances;
	Instances = 0;

	return true;
}

void InstancedModelClass::CacheInstanceVector(ID3D11Device* device, const vector<D3DXVECTOR3>& instanceVector)
{
	m_vecInstances = instanceVector;
}

void InstancedModelClass::ShutdownBuffers()
{
	Super::ShutdownBuffers();

	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = nullptr;
	}
}

void InstancedModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* bufferPointers[2] { m_vertexBuffer, m_instanceBuffer };

	unsigned int strides[2] { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offsets[2] { 0, 0 };

	// 입력 조립기에 정점 & 인스턴싱 버퍼를 세팅
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// 위상 구조 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool InstancedModelClass::CreateInstanceBuffer(ID3D11Device* device, void* vertices)
{
	HRESULT result;

	D3D11_BUFFER_DESC InstanceBufferDesc;
	InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	InstanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.CPUAccessFlags = 0;
	InstanceBufferDesc.MiscFlags = 0;
	InstanceBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = vertices;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// 인스턴스 버퍼 생성
	result = device->CreateBuffer(&InstanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}
