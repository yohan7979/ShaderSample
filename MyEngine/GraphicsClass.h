#pragma once

#include "d3dclass.h"
#include "Types.h"

class ShaderManagerClass;
class LightManagerClass;
class CameraClass;
class PositionClass;
class TerrainClass;
class ModelClass;
class InstancedModelClass;
class BitmapClass;
class DebugWindowClass;
class TextClass;
class RenderTextureClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int mouseX, int mouseY, int fps, int cpu, float frameTime, class InputClass* input);
	bool Render(float frameTime);
	bool HandleInput(float frameTime, class InputClass* input);

private:
	bool RenderToTexture(float frameTime);
	bool RenderToShadowMap(float frameTime);
	bool RenderScene(float frameTime);

private:
	D3DClass* m_D3D;
	ShaderManagerClass* m_ShaderManager;
	LightManagerClass* m_LightManager;

	CameraClass* m_Camera;
	CameraClass* m_UICamera;
	PositionClass* m_Position;
	TerrainClass* m_Terrain;

	InstancedModelClass* m_SphereModel;
	ModelClass* m_SphereModel2;
	ModelClass* m_PlaneModel;
	ModelClass* m_PlaneModel2;

	BitmapClass* m_Bitmap;
	DebugWindowClass* m_DebugWindow;
	TextClass* m_Text;

	RenderTextureClass* m_RenderTexture;
	RenderTextureClass* m_ShadowTexture;
	RenderTextureClass* m_ReflectionTexture;


};
