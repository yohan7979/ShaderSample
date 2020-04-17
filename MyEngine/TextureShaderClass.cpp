#include "TextureShaderClass.h"



TextureShaderClass::TextureShaderClass()
{
	m_matrixBuffer = 0;
	m_sampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass & other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../MyEngine/Shader/texture.vs", L"../MyEngine/Shader/texture.ps",
		"TextureVertexShader", "TexturePixelShader");
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	ShaderClass::Shutdown();

	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float alphaBlend /*= 1.f*/)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, alphaBlend);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename,
	const char* vsName, const char* psName)
{
	Super::InitializeShader(device, hwnd, vsFilename, psFilename, vsName, psName);

	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
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

	D3D11_BUFFER_DESC TransparentBufferDesc;
	TransparentBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TransparentBufferDesc.ByteWidth = sizeof(TransparentBufferType);
	TransparentBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TransparentBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TransparentBufferDesc.MiscFlags = 0;
	TransparentBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&TransparentBufferDesc, NULL, &m_TransparentBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the Transparent constant buffer.
	if (m_TransparentBuffer)
	{
		m_TransparentBuffer->Release();
		m_TransparentBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}


bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float alphaBlend /*= 1.f*/)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;


	// ���̴��� �ѱ������ ����� ��ġ����
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// {{ ������ ��� ����
	result = deviceContext->Map(m_TransparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	TransparentBufferType* dataPtr2 = static_cast<TransparentBufferType*>(mappedResource.pData);
	dataPtr2->alpha = alphaBlend;
	deviceContext->Unmap(m_TransparentBuffer, 0);
	// }} ������ ��� ����

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// �ȼ� ���̴��� set
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_TransparentBuffer);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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

	return;
}