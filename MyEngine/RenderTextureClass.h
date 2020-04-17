#pragma once
#include <d3d11.h>
#include <d3dx10math.h>

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass& other);
	~RenderTextureClass();

	bool Initialize(ID3D11Device * device, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a);
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView; }

	void GetProjectionMatrix(D3DXMATRIX& projectionMatrix);
	void GetOrthoMatrix(D3DXMATRIX& orthoMatrix);

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;
};

