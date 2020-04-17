#include "LightShaderClass.h"



LightShaderClass::LightShaderClass()
{
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_cameraBuffer = 0;
	m_pointlightColorBuffer = 0;
	m_pointlightPosBuffer = 0;
}

LightShaderClass::LightShaderClass(const LightShaderClass & other)
{
}


LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// Initialize the vertex and pixel shaders.
	return InitializeShader(device, hwnd, L"../MyEngine/Shader/light.vs", L"../MyEngine/Shader/light.ps",
		"LightVertexShader", "LightPixelShader");
}

void LightShaderClass::Shutdown()
{
	ShaderClass::Shutdown();

	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
	D3DXVECTOR4 specularColor, float specularPower, D3DXVECTOR3 cameraPosition,
	D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[])
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
		texture, lightDirection, ambientColor, diffuseColor, specularColor, specularPower, cameraPosition,
		pointlightPos, pointlightColor);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename,
	const char* vsName, const char* psName)
{
	ShaderClass::InitializeShader(device, hwnd, vsFilename, psFilename, vsName, psName);

	HRESULT result;
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	m_vertexShaderBuffer->Release();
	m_vertexShaderBuffer = 0;

	m_pixelShaderBuffer->Release();
	m_pixelShaderBuffer = 0;

	// Create a texture sampler state description.
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

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}


	// 카메라 상수 버퍼 구조체를 정의한다.
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
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

void LightShaderClass::ShutdownShader()
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

	// Release the camera constant buffer.
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
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


bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
	D3DXVECTOR4 diffuseColor, D3DXVECTOR4 specularColor, float specularPower, D3DXVECTOR3 cameraPosition,
	D3DXVECTOR4 pointlightPos[], D3DXVECTOR4 pointlightColor[])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;
	PointLightPosBufferType* dataPtr4;
	PointLightColorBufferType* dataPtr5;


	// 행렬 상수버퍼를 갱신한다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	deviceContext->Unmap(m_matrixBuffer, 0);

	// VS의 레지스터 0번 설정
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	
	// 카메라 상수버퍼를 갱신한다.
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr3 = (CameraBufferType*)mappedResource.pData;
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.f;
	deviceContext->Unmap(m_cameraBuffer, 0);

	// VS의 레지스터 1번 설정
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);


	// PS에게 텍스처를 넘긴다.
	deviceContext->PSSetShaderResources(0, 1, &texture);



	// 조명 상수버퍼를 갱신한다.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr2 = (LightBufferType*)mappedResource.pData;
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;
	deviceContext->Unmap(m_lightBuffer, 0);

	// PS의 레지스터 0번 설정
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);


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

	// VS의 레지스터 2번 설정
	bufferNumber = 2;
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

	return true;
}


void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}