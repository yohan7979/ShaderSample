#include "ShaderClass.h"


ShaderClass::ShaderClass()	:
	m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0)
{
}

ShaderClass::ShaderClass(const ShaderClass & other)
{
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return true;
}

void ShaderClass::Shutdown()
{
	ShutdownShader();
}

bool ShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFilename, WCHAR * psFilename,
	const char* vsName, const char* psName)
{
	HRESULT result;

	// Initialize the pointers this function will use to null.
	m_errorMessage = 0;
	m_vertexShaderBuffer = 0;
	m_pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, vsName, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&m_vertexShaderBuffer, &m_errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (m_errorMessage)
		{
			OutputShaderErrorMessage(m_errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, psName, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&m_pixelShaderBuffer, &m_errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (m_errorMessage)
		{
			OutputShaderErrorMessage(m_errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}


	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(m_pixelShaderBuffer->GetBufferPointer(), m_pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob * m_errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	char* compileErrors;
	size_t bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(m_errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = m_errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	m_errorMessage->Release();
	m_errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

void ShaderClass::ShutdownShader()
{
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

void ShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
}
