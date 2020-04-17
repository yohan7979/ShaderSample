#include "TextClass.h"



TextClass::TextClass()	:
	m_Font(0),
	m_FontShader(0)
{
	m_vecSentence.reserve(TEXT_END);
}

TextClass::TextClass(const TextClass & other)
{
}


TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
	D3DXMATRIX baseViewMatrix)
{
	bool result;

	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Initialize the font object.
	result = m_Font->Initialize(device, "../MyEngine/Resource/fontdata.txt", L"../MyEngine/Resource/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�Ʈ ���� �ʱ�ȭ
	for (int i = 0; i < TEXT_END; ++i)
	{
		SentenceType* sentence;
		result = InitializeSentence(&sentence, 50, device);
		if (!result)
		{
			return false;
		}
		m_vecSentence.push_back(sentence);
	}

	return true;
}

void TextClass::Shutdown()
{
	// Release Text Sentence Vector
	vector<SentenceType*>::iterator iter;
	vector<SentenceType*>::iterator iterEnd = m_vecSentence.end();

	for (iter = m_vecSentence.begin(); iter != iterEnd; ++iter)
	{
		ReleaseSentence(&(*iter));
	}

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	vector<SentenceType*>::iterator iter;
	vector<SentenceType*>::iterator iterEnd = m_vecSentence.end();

	for (iter = m_vecSentence.begin(); iter != iterEnd; ++iter)
	{
		result = RenderSentence(deviceContext, *iter, worldMatrix, orthoMatrix);
		if (!result)
		{
			return false;
		}
	}

	return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Color ����
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// ���ڿ��� ũ��(���� ����)
	numLetters = (int)strlen(text);

	// ���� �����÷ο� üũ
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix,
	D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
		pixelColor);
	if (!result)
	{
		false;
	}

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;


	// MouseX �������� ���ڿ��� �ٲ۴�.
	_itoa_s(mouseX, tempString, 10);

	// Mouse X : value
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[MOUSE_X], mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// MouseY �������� ���ڿ��� �ٲ۴�.
	_itoa_s(mouseY, tempString, 10);

	// Mouse Y: value
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[MOUSE_Y], mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext * deviceContext)
{
	char fpsString[16];
	float red, green, blue;
	bool result;

	if (fps > 9999)
	{
		fps = 9999;
	}
	
	// fps�� ���ڿ��� �ٲ۴�.
	sprintf_s(fpsString, "Fps : %d", fps);

	if (fps >= 60)
	{
		red = 0.f;
		green = 1.f;
		blue = 0.f;
	}
	else if (fps < 60)
	{
		red = 1.f;
		green = 1.f;
		blue = 0.f;
	}
	else if (fps < 30)
	{
		red = 1.f;
		green = 0.f;
		blue = 0.f;
	}

	result = UpdateSentence(m_vecSentence[FPS] , fpsString, 20, 60, red, green, blue, deviceContext);
	if (!result)
	{
		return false;
	}

	
	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext * deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[CPU], cpuString, 20, 80, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetVideoCardInfo(char * videoCardName, int videoCardMemory, ID3D11DeviceContext * deviceContext)
{
	bool result;
	char dataString[50];
	char memoryString[32];
	char tempString[16];


	// Setup the video card name string.
	strcpy_s(dataString, "Video Card: ");
	strcat_s(dataString, videoCardName);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[VIDEO_NAME], dataString, m_screenWidth - 220, 20, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Truncate the memory value to prevent buffer over flow.
	if (videoCardMemory > 9999999)
	{
		videoCardMemory = 9999999;
	}

	// Convert the video memory integer value to a string format.
	_itoa_s(videoCardMemory, tempString, 10);

	// Setup the video memory string.
	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[VIDEO_MEMORY], memoryString, m_screenWidth - 220, 40, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCameraPosition(int x, int y, int z, ID3D11DeviceContext * deviceContext)
{
	bool result;
	char tempString[20];
	char cameraString[40];

	
	// Setup the video card name string.
	strcpy_s(cameraString, "Camera Position : ");

	_itoa_s(x, tempString, 10);
	strcat_s(cameraString, tempString);
	strcat_s(cameraString, ", ");

	_itoa_s(y, tempString, 10);
	strcat_s(cameraString, tempString);
	strcat_s(cameraString, ", ");

	_itoa_s(z, tempString, 10);
	strcat_s(cameraString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[CAMERA_POS], cameraString, m_screenWidth - 220, 60, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCameraRotation(int x, int y, int z, ID3D11DeviceContext * deviceContext)
{
	bool result;
	char tempString[20];
	char cameraString[40];


	// Setup the video card name string.
	strcpy_s(cameraString, "Camera Rotation : ");

	_itoa_s(x, tempString, 10);
	strcat_s(cameraString, tempString);
	strcat_s(cameraString, ", ");

	_itoa_s(y, tempString, 10);
	strcat_s(cameraString, tempString);
	strcat_s(cameraString, ", ");

	_itoa_s(z, tempString, 10);
	strcat_s(cameraString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_vecSentence[CAMERA_ROT], cameraString, m_screenWidth - 220, 80, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetRenderCount(int count, ID3D11DeviceContext* deviceContext)
{
	char tempString[32]{};

	sprintf_s(tempString, "Render count : %d", count);

	if (!UpdateSentence(m_vecSentence[RENDER_OBJECT_COUNT], tempString, m_screenWidth - 220, 100, 1.f, 1.f, 1.f, deviceContext))
	{
		return false;
	}
	
	return true;
}
