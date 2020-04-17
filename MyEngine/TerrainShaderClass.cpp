#include "TerrainShaderClass.h"



TerrainShaderClass::TerrainShaderClass()	:
	m_sampleState(0),
	m_matrixBuffer(0),
	m_lightBuffer(0),
	m_pointlightColorBuffer(0),
	m_pointlightPosBuffer(0)
{
}

TerrainShaderClass::TerrainShaderClass(const TerrainShaderClass & other)
{
}


TerrainShaderClass::~TerrainShaderClass()
{
}

bool TerrainShaderClass::Initialize(ID3D11Device * device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../MyEngine/Shader/terrain.vs", L"../MyEngine/Shader/terrain.ps",
		"TerrainVertexShader", "TerrainPixelShader");
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainShaderClass::Shutdown()
{
	ShaderClass::Shutdown();

	ShutdownShader();
}

bool TerrainShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
	D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDir, ID3D11ShaderResourceView* texture,
	D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[])
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, 
		lightDir, texture, pointlightPos, pointlightColor);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool TerrainShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename,
	const char* vsName, const char* psName)
{
	ShaderClass::InitializeShader(device, hwnd, vsFilename, psFilename, vsName, psName);

	HRESULT result;
	
	// 입력 조립기 생성
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	result = device->CreateInputLayout(polygonLayout, numElements, m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	m_vertexShaderBuffer->Release();
	m_vertexShaderBuffer = 0;

	m_pixelShaderBuffer->Release();
	m_pixelShaderBuffer = 0;

	// 샘플러 생성
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼 생성
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 포인트 라이트 상수 버퍼
	D3D11_BUFFER_DESC pointlightColorBufferDesc;
	pointlightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointlightColorBufferDesc.ByteWidth = sizeof(PointLightColorBufferType);
	pointlightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointlightColorBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	pointlightColorBufferDesc.MiscFlags = 0;
	pointlightColorBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&pointlightColorBufferDesc, NULL, &m_pointlightColorBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC pointlightPosBufferDesc;
	pointlightPosBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointlightPosBufferDesc.ByteWidth = sizeof(PointLightPosBufferType);
	pointlightPosBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointlightPosBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	pointlightPosBufferDesc.MiscFlags = 0;
	pointlightPosBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&pointlightPosBufferDesc, NULL, &m_pointlightPosBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TerrainShaderClass::ShutdownShader()
{
	if (m_pointlightPosBuffer)
	{
		m_pointlightPosBuffer->Release();
		m_pointlightPosBuffer = 0;
	}

	if (m_pointlightColorBuffer)
	{
		m_pointlightColorBuffer->Release();
		m_pointlightColorBuffer = 0;
	}

	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}
}

bool TerrainShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDir,
	ID3D11ShaderResourceView* texture, D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int bufferNumber;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	MatrixBufferType* dataPtr_Matrix;
	dataPtr_Matrix = (MatrixBufferType*)mappedResource.pData;
	dataPtr_Matrix->world = worldMatrix;
	dataPtr_Matrix->view = viewMatrix;
	dataPtr_Matrix->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);



	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	LightBufferType* dataPtr_Light;
	dataPtr_Light = (LightBufferType*)mappedResource.pData;
	dataPtr_Light->ambientColor = ambientColor;
	dataPtr_Light->diffuseColor = diffuseColor;
	dataPtr_Light->lightDirection = lightDir;
	dataPtr_Light->padding = 0.f;

	deviceContext->Unmap(m_lightBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);


	PointLightPosBufferType* dataPtr4;
	PointLightColorBufferType* dataPtr5;
	// 포인트 라이트 상수버퍼 갱신
	result = deviceContext->Map(m_pointlightPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr4 = (PointLightPosBufferType*)mappedResource.pData;
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		dataPtr4->pointlightPos[i] = pointlightPos[i];
	}
	deviceContext->Unmap(m_pointlightPosBuffer, 0);

	// VS의 레지스터 1번 설정
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pointlightPosBuffer);


	result = deviceContext->Map(m_pointlightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr5 = (PointLightColorBufferType*)mappedResource.pData;
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		dataPtr5->pointlightColor[i] = pointlightColor[i];
	}
	deviceContext->Unmap(m_pointlightColorBuffer, 0);

	// PS의 레지스터 1번 설정
	bufferNumber = 1;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pointlightColorBuffer);



	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TerrainShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
