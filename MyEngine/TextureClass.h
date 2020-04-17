#pragma once
#include <d3d11.h>
#include <d3dx11tex.h>
#include <vector>
#include <string>

using namespace std;

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass& other);
	~TextureClass();

	bool Initialize(ID3D11Device*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};


