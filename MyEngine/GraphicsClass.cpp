#include "GraphicsClass.h"
#include "InputClass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "TerrainClass.h"
#include "PositionClass.h"
#include "RenderTextureClass.h"
#include "DebugWindowClass.h"
#include "ShaderManagerClass.h"
#include "LightManagerClass.h"
#include "ReflectionShaderClass.h"
#include "InstancedModelClass.h"

GraphicsClass::GraphicsClass() :
	m_D3D(0),
	m_Camera(0),
	m_UICamera(0),
	m_SphereModel(0),
	m_SphereModel2(0),
	m_PlaneModel(0),
	m_Bitmap(0),
	m_Text(0),
	m_Terrain(0),
	m_Position(0),
	m_RenderTexture(0),
	m_DebugWindow(0),
	m_ShadowTexture(0),
	m_ReflectionTexture(0),
	m_ShaderManager(0),
	m_LightManager(0)
{
}

GraphicsClass::GraphicsClass(const GraphicsClass &other)
{
}


GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// D3D 객체 생성
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// D3D 초기화
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 클래스 생성
	float cameraX = 0.f, cameraY = 5.f, cameraZ = -10.f;
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}
	
	m_Camera->CalculateMatrix(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	m_Camera->SetPosition(cameraX, cameraY, cameraZ);

	m_UICamera = new CameraClass;
	if (!m_UICamera)
	{
		return false;
	}
	D3DXMATRIX baseViewMatrix;
	m_UICamera->SetPosition(0.0f, 0.0f, -1.0f);
	m_UICamera->Render();
	m_UICamera->GetViewMatrix(baseViewMatrix);

	// 카메라 포지션 클래스
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(cameraX, cameraY, cameraZ);

	// 모델 클래스 생성
	m_SphereModel = new InstancedModelClass;
	if (!m_SphereModel)
	{
		return false;
	}
	// 모델 클래스 초기화
	vector<D3DXVECTOR3> instancePositions;
	instancePositions.push_back(D3DXVECTOR3(2.f, 0.f, 0.f));
	instancePositions.push_back(D3DXVECTOR3(2.f, 2.f, 0.f));
	instancePositions.push_back(D3DXVECTOR3(4.f, 0.f, 0.f));
	instancePositions.push_back(D3DXVECTOR3(4.f, 2.f, 0.f));
	result = m_SphereModel->Initialize<WCHAR>(m_D3D->GetDevice(), "../MyEngine/Resource/Sphere.txt", L"../MyEngine/Resource/stone01.dds", L"../MyEngine/Resource/bump01.dds", instancePositions);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	m_SphereModel2 = new ModelClass;
	if (!m_SphereModel2)
	{
		return false;
	}
	// 모델 클래스 초기화
	result = m_SphereModel2->Initialize<WCHAR>(m_D3D->GetDevice(), "../MyEngine/Resource/Sphere.txt", L"../MyEngine/Resource/aa.bmp");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 모델 클래스 생성
	m_PlaneModel = new ModelClass;
	if (!m_PlaneModel)
	{
		return false;
	}
	// 모델 클래스 초기화
	result = m_PlaneModel->Initialize<WCHAR>(m_D3D->GetDevice(), "../MyEngine/Resource/plane01.txt", L"../MyEngine/Resource/wall01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 모델 클래스 생성
	m_PlaneModel2 = new ModelClass;
	if (!m_PlaneModel2)
	{
		return false;
	}
	// 모델 클래스 초기화
	result = m_PlaneModel2->Initialize<WCHAR>(m_D3D->GetDevice(), "../MyEngine/Resource/plane01.txt", L"../MyEngine/Resource/stone01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	// 조명 객체 생성 및 초기화
	m_LightManager = new LightManagerClass;
	m_LightManager->Initialize();


	// 비트맵 객체 생성 및 초기화
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../MyEngine/Resource/aa.bmp", 256, 256);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// 텍스트 객체 생성 및 초기화
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// 터레인 객체 생성 및 초기화
	m_Terrain = new TerrainClass;
	if (!m_Terrain)
	{
		return false;
	}
	result = m_Terrain->Initialize(m_D3D->GetDevice(), "../MyEngine/Resource/heightmap01.bmp", L"../MyEngine/Resource/dirt01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// 렌더 텍스처 생성
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		return false;
	}

	// 디버그 윈도우 생성
	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow)
	{
		return false;
	}
	result = m_DebugWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, 100, 100);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}


	// 그림자 맵 생성
	m_ShadowTexture = new RenderTextureClass;
	if (!m_ShadowTexture)
	{
		return false;
	}
	result = m_ShadowTexture->Initialize(m_D3D->GetDevice(), SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, SHADOW_MAP_DEPTH, SHADOW_MAP_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 반사 텍스처 생성
	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture)
	{
		return false;
	}
	result = m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		return false;
	}


	// 쉐이더 매니저 생성
	m_ShaderManager = new ShaderManagerClass;
	m_ShaderManager->Initialize(m_D3D->GetDevice(), hwnd);



	// 비디오 카드 텍스트
	char videoCard[128];
	int videoMemory;
	m_D3D->GetVideoCardInfo(videoCard, videoMemory);
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_D3D->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_LightManager)
	{
		m_LightManager->Shutdown();
		delete m_LightManager;
		m_LightManager = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_ShadowTexture)
	{
		m_ShadowTexture->Shutdown();
		delete m_ShadowTexture;
		m_ShadowTexture = 0;
	}

	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	if (m_SphereModel)
	{
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = 0;
	}

	if (m_SphereModel2)
	{
		m_SphereModel2->Shutdown();
		delete m_SphereModel2;
		m_SphereModel2 = 0;
	}

	if (m_PlaneModel)
	{
		m_PlaneModel->Shutdown();
		delete m_PlaneModel;
		m_PlaneModel = 0;
	}

	if (m_PlaneModel2)
	{
		m_PlaneModel2->Shutdown();
		delete m_PlaneModel2;
		m_PlaneModel2 = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_UICamera)
	{
		delete m_UICamera;
		m_UICamera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
}

bool GraphicsClass::Frame(int mouseX, int mouseY, int fps, int cpu, float frameTime, InputClass* input)
{
	bool result;
	static float lightAngle = 240.f;
	float radians;
	static float lightPosX = 6.f;

	lightPosX -= 0.003f * frameTime;
	lightAngle -= 0.03f * frameTime;
	if (lightAngle < 90.f)
	{
		lightAngle = 270.f;
		lightPosX = 9.f;
	}
	radians = lightAngle * 0.0174532925f;
	
	DirectionalLightClass* pLightDir = static_cast<DirectionalLightClass*>(m_LightManager->GetLight("DirectionalLight"));
	pLightDir->SetDirection(sinf(radians), cosf(radians), 0.f);
	pLightDir->SetPosition(lightPosX, 8.f, -0.1f);
	pLightDir->SetLookAt(-lightPosX, 0.f, 0.f);

	// 마우스 좌표를 텍스트로 나타낸다.
	m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());

	// Fps와 Cpu를 텍스트로 나타낸다.
	m_Text->SetFps(fps, m_D3D->GetDeviceContext());
	m_Text->SetCpu(cpu, m_D3D->GetDeviceContext());

	// Do the frame input processing.
	result = HandleInput(frameTime, input);
	if (!result)
	{
		return false;
	}
	// 씬을 그린다.
	result = Render(frameTime);
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float frameTime)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;

	// 먼저 텍스처에 그린다.
	if (!RenderToTexture(frameTime))
	{
		return false;
	}

	// 그림자 맵을 그린다.
	if (!RenderToShadowMap(frameTime))
	{
		return false;
	}


	// 씬 그리기를 시작한다. (백버퍼)
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	if (!RenderScene(frameTime))
	{
		return false;
	}

	// 월드, 뷰(UI카메라), 직교 행렬을 얻어온다.
	m_Camera->GetWorldMatrix(worldMatrix);
	m_UICamera->GetViewMatrix(viewMatrix);
	m_Camera->GetOrthoMatrix(orthoMatrix);

	// Z-버퍼를 끄고 직교 렌더링 수행
	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();

	// 텍스트 그리기
	if (!m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// 디버그 윈도우 그리기
	if (!m_DebugWindow->Render(m_D3D->GetDeviceContext(), 100, 10))
	{
		return false;
	}

	TextureShaderClass* TextureShader = m_ShaderManager->GetShader<TextureShaderClass>("TextureShader");
	if (TextureShader)
	{
		bool result = TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView());
		if (!result)
		{
			return false;
		}

	}

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::HandleInput(float frameTime, InputClass* input)
{
	bool keyDown, result;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = input->IsWPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = input->IsSPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = input->IsAPressed();
	m_Position->LookUpward(keyDown);

	keyDown = input->IsDPressed();
	m_Position->LookDownward(keyDown);

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(static_cast<int>(posX), static_cast<int>(posY), static_cast<int>(posZ), m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(static_cast<int>(rotX), static_cast<int>(rotY), static_cast<int>(rotZ), m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::RenderToTexture(float frameTime)
{
	bool result;

	// 텍스처를 렌더 타겟으로 설정한다.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// 렌더 타겟을 클리어한다.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.f, 0.f, 1.f, 1.f);


	// 텍스처에 씬을 그린다.
	result = RenderScene(frameTime);
	if (!result)
	{
		return false;
	}
	// 렌더 타겟을 본래 백버퍼로 재설정한다.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderToShadowMap(float frameTime)
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;

	// 행렬을 얻어온다.
	DirectionalLightClass* pLightDir = static_cast<DirectionalLightClass*>(m_LightManager->GetLight("DirectionalLight"));
	pLightDir->GenerateViewMatrix();
	pLightDir->GetViewMatrix(lightViewMatrix);
	pLightDir->GetOrthoMatrix(lightOrthoMatrix);
	m_Camera->GetWorldMatrix(worldMatrix);

	m_ShadowTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_ShadowTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	D3DXMATRIX matrixRot, matrixTrans;
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * frameTime * 0.0005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	D3DXMatrixRotationY(&matrixRot, rotation);
	D3DXMatrixTranslation(&matrixTrans, 2.0f, 4.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &matrixRot, &matrixTrans);

	// 모델의 정점, 인덱스 버퍼를 세팅한다.
	m_SphereModel->Render(m_D3D->GetDeviceContext());

	// 쉐이더를 이용해 그린다.
	DepthShaderClass* DepthShader = m_ShaderManager->GetShader<DepthShaderClass>("DepthShader");
	if (DepthShader)
	{
		bool result = DepthShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
		if (!result)
		{
			return false;
		}
	}

	m_Camera->GetWorldMatrix(worldMatrix);
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 3.0f, 0.0f);

	// 모델의 정점, 인덱스 버퍼를 세팅한다.
	m_PlaneModel->Render(m_D3D->GetDeviceContext());

	// 쉐이더를 이용해 그린다.
	if (DepthShader)
	{
		bool result = DepthShader->Render(m_D3D->GetDeviceContext(), m_PlaneModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
		if (!result)
		{
			return false;
		}
	}

	m_D3D->SetBackBufferRenderTarget();
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderScene(float frameTime)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix, reflectionViewMatrix;
	D3DXMATRIX lightViewMatrix, lightOrthoMatrix;
	bool result;

	// 디렉셔널 라이트
	DirectionalLightClass* pLightDir = static_cast<DirectionalLightClass*>(m_LightManager->GetLight("DirectionalLight"));

	// 포인트 라이트
	D3DXVECTOR3 lightPosition = pLightDir->GetPosition();
	D3DXVECTOR4 pointlightPos[NUM_LIGHTS];
	D3DXVECTOR4 pointlightColor[NUM_LIGHTS];
	char lightName[20] = {};
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		sprintf_s(lightName, "PointLight%d", i + 1);
		pointlightPos[i] = m_LightManager->GetLight(lightName)->GetPositionVector4();
		pointlightColor[i] = m_LightManager->GetLight(lightName)->GetDiffuseColor();
	}

	// 카메라 행렬 갱신
	m_Camera->Render();

	// 행렬을 얻어온다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Camera->GetWorldMatrix(worldMatrix);
	m_Camera->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);
	pLightDir->GetViewMatrix(lightViewMatrix);
	pLightDir->GetOrthoMatrix(lightOrthoMatrix);

	// 터레인을 칼라 쉐이더를 이용해서 그리기
	D3DXMatrixTranslation(&worldMatrix, -128.0f, 0.0f, -128.0f);
	m_Terrain->Render(m_D3D->GetDeviceContext());
	TerrainShaderClass* TerrainShader = m_ShaderManager->GetShader<TerrainShaderClass>("TerrainShader");
	if (TerrainShader)
	{
		result = TerrainShader->Render(
			m_D3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
			pLightDir->GetAmbientColor(), pLightDir->GetDiffuseColor(), pLightDir->GetDirection(), 
			m_Terrain->GetTexture(), pointlightPos, pointlightColor
		);

		if (!result)
		{
			return false;
		}
	}

	// 월드 행렬 회전시키기
	D3DXMATRIX matrixRot, matrixTrans;
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * frameTime * 0.0005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	m_Camera->GetWorldMatrix(worldMatrix);
	D3DXMatrixRotationY(&matrixRot, rotation);
	D3DXMatrixTranslation(&matrixTrans, 2.0f, 4.0f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &matrixRot, &matrixTrans);

	// 모델의 정점, 인덱스 버퍼를 세팅한다.
	int rendercount = 0;
	Frustrum fs = m_Camera->GetFrustrum();

	m_SphereModel->Render(m_D3D->GetDeviceContext());
	BumpMapShaderClass* BumpShader = m_ShaderManager->GetShader<BumpMapShaderClass>("BumpMapShader");
	if (BumpShader)
	{
		if (fs.IsInSphere(D3DXVECTOR3(2.f, 4.f, 0.f), 1.f))
		{
		result = BumpShader->Render(
			m_D3D->GetDeviceContext(), m_SphereModel->GetVertexCount(), m_SphereModel->GetIndexCount(), m_SphereModel->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix,
			m_SphereModel->GetTextures(), m_SphereModel->GetTextureCount(), pLightDir->GetDirection(), pLightDir->GetDiffuseColor()
		);

		if (!result)
		{
			return false;
		}

			rendercount += 1;
		}
	}
	
	m_Camera->GetWorldMatrix(worldMatrix);
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 3.0f, 0.0f);
	m_PlaneModel->Render(m_D3D->GetDeviceContext());
	ShadowShaderClass* ShadowShader = m_ShaderManager->GetShader<ShadowShaderClass>("ShadowShader");
	if (ShadowShader)
	{
		if (fs.IsInSphere(D3DXVECTOR3(0.0f, 3.0f, 0.0f), 10.f))
		{
		result = ShadowShader->Render(
			m_D3D->GetDeviceContext(), m_PlaneModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			lightViewMatrix, lightOrthoMatrix, m_PlaneModel->GetTexture(), m_ShadowTexture->GetShaderResourceView(),
			pLightDir->GetDirection(), pLightDir->GetAmbientColor(), pLightDir->GetDiffuseColor()
		);

		if (!result)
		{
			return false;
		}

	
		rendercount += 1;
		}
	}

	m_Camera->GetWorldMatrix(worldMatrix);
	D3DXMatrixTranslation(&worldMatrix, lightPosition.x, lightPosition.y, lightPosition.z);
	m_SphereModel2->Render(m_D3D->GetDeviceContext());
	LightShaderClass* LightShader = m_ShaderManager->GetShader<LightShaderClass>("LightShader");
	if (LightShader)
	{
		if (fs.IsInSphere(D3DXVECTOR3(lightPosition.x, lightPosition.y, lightPosition.z), 1.f))
		{
		result = LightShader->Render(
			m_D3D->GetDeviceContext(), m_SphereModel2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			m_SphereModel2->GetTexture(),
			pLightDir->GetDirection(), pLightDir->GetAmbientColor(), pLightDir->GetDiffuseColor(), pLightDir->GetSpecularColor(),
			pLightDir->GetSpecularPower(), m_Camera->GetPosition(), pointlightPos, pointlightColor
		);

		if (!result)
		{
			return false;
		}


			rendercount += 1;
		}
	}

	m_Text->SetRenderCount(rendercount, m_D3D->GetDeviceContext());

	return true;
}

