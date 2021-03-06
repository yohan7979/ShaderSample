#include "CameraClass.h"



CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass & other)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float radians;

	// 위쪽을 가리키는 벡터를 만듭니다. 
	up.x = 0.0f; 
	up.y = 1.0f; 
	up.z = 0.0f;

	// 월드에 카메라 위치를 설정합니다.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// 회전을 라디안 값으로 계산합니다. 
	radians = m_rotationY * 0.0174532925f;

	// 카메라가 보는 방향을 설정합니다. 
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_positionZ;

	// 위의 세 벡터를 이용하여 뷰 행렬을 생성합니다. 
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	// 절두체 계산
	CalculateFrustrum();
}

void CameraClass::RenderReflection(float height)
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// 반사를 위해 카메라 y값을 역전시킴
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.f);
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_reflectionViewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void CameraClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void CameraClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void CameraClass::GetReflectionViewMatrix(D3DXMATRIX& reflectionMatrix)
{
	reflectionMatrix = m_reflectionViewMatrix;
}

bool CameraClass::CalculateMatrix(int screenWidth, int screenHeight, float screenNear, float screenDepth)
{
	// 행렬 계산
	float fieldOfView, screenAspect;

	// Setup the projection matrix.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
	D3DXMatrixIdentity(&m_worldMatrix);
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void CameraClass::CalculateFrustrum()
{
	// 뷰 * 원근 투영 행렬의 역행렬 구하기
	D3DXMATRIX ViewProjMatrix = m_viewMatrix * m_projectionMatrix;
	float Determinant = D3DXMatrixDeterminant(&ViewProjMatrix);

	D3DXMATRIX InverseMatrix;
	D3DXMatrixInverse(&InverseMatrix, &Determinant, &ViewProjMatrix);

	// 뷰포트 공간
	D3DXVECTOR3 PlaneVertices[8];

	// 앞면
	PlaneVertices[0] = D3DXVECTOR3(-1.f, 1.f, 0.f);
	PlaneVertices[1] = D3DXVECTOR3(1.f, 1.f, 0.f);
	PlaneVertices[2] = D3DXVECTOR3(1.f, -1.f, 0.f);
	PlaneVertices[3] = D3DXVECTOR3(-1.f, -1.f, 0.f);

	// 뒷면
	PlaneVertices[4] = D3DXVECTOR3(-1.f, 1.f, 1.f);
	PlaneVertices[5] = D3DXVECTOR3(1.f, 1.f, 1.f);
	PlaneVertices[6] = D3DXVECTOR3(1.f, -1.f, 1.f);
	PlaneVertices[7] = D3DXVECTOR3(-1.f, -1.f, 1.f);

	// 역행렬을 곱하여 절두체의 각 정점을 월드 공간으로 변환
	D3DXVECTOR3 v;
	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&v, &PlaneVertices[i], &InverseMatrix);
		m_Frustrum.vertex[i] = v;
	}

	// 센터 계산
	m_Frustrum.center.x = (m_Frustrum.vertex[0].x + m_Frustrum.vertex[1].x) / 2;
	m_Frustrum.center.y = (m_Frustrum.vertex[0].y + m_Frustrum.vertex[3].y) / 2;
	m_Frustrum.center.z = (m_Frustrum.vertex[0].z + m_Frustrum.vertex[4].z) / 2;

	// 정점으로 각 평면을 구한다.
	// 바깥쪽으로 노말을 지정하기 위해 시계 방향으로
	D3DXPlaneFromPoints(&m_Frustrum.plane[0], &m_Frustrum.vertex[0], &m_Frustrum.vertex[1], &m_Frustrum.vertex[2]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[1], &m_Frustrum.vertex[4], &m_Frustrum.vertex[7], &m_Frustrum.vertex[6]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[2], &m_Frustrum.vertex[0], &m_Frustrum.vertex[3], &m_Frustrum.vertex[7]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[3], &m_Frustrum.vertex[1], &m_Frustrum.vertex[5], &m_Frustrum.vertex[6]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[4], &m_Frustrum.vertex[0], &m_Frustrum.vertex[4], &m_Frustrum.vertex[5]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[5], &m_Frustrum.vertex[3], &m_Frustrum.vertex[2], &m_Frustrum.vertex[6]);
}
