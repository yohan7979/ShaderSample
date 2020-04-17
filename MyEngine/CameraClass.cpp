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

	// ������ ����Ű�� ���͸� ����ϴ�. 
	up.x = 0.0f; 
	up.y = 1.0f; 
	up.z = 0.0f;

	// ���忡 ī�޶� ��ġ�� �����մϴ�.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// ȸ���� ���� ������ ����մϴ�. 
	radians = m_rotationY * 0.0174532925f;

	// ī�޶� ���� ������ �����մϴ�. 
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_positionZ;

	// ���� �� ���͸� �̿��Ͽ� �� ����� �����մϴ�. 
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	// ����ü ���
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

	// �ݻ縦 ���� ī�޶� y���� ������Ŵ
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
	// ��� ���
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
	// �� * ���� ���� ����� ����� ���ϱ�
	D3DXMATRIX ViewProjMatrix = m_viewMatrix * m_projectionMatrix;
	float Determinant = D3DXMatrixDeterminant(&ViewProjMatrix);

	D3DXMATRIX InverseMatrix;
	D3DXMatrixInverse(&InverseMatrix, &Determinant, &ViewProjMatrix);

	// ����Ʈ ����
	D3DXVECTOR3 PlaneVertices[8];

	// �ո�
	PlaneVertices[0] = D3DXVECTOR3(-1.f, 1.f, 0.f);
	PlaneVertices[1] = D3DXVECTOR3(1.f, 1.f, 0.f);
	PlaneVertices[2] = D3DXVECTOR3(1.f, -1.f, 0.f);
	PlaneVertices[3] = D3DXVECTOR3(-1.f, -1.f, 0.f);

	// �޸�
	PlaneVertices[4] = D3DXVECTOR3(-1.f, 1.f, 1.f);
	PlaneVertices[5] = D3DXVECTOR3(1.f, 1.f, 1.f);
	PlaneVertices[6] = D3DXVECTOR3(1.f, -1.f, 1.f);
	PlaneVertices[7] = D3DXVECTOR3(-1.f, -1.f, 1.f);

	// ������� ���Ͽ� ����ü�� �� ������ ���� �������� ��ȯ
	D3DXVECTOR3 v;
	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&v, &PlaneVertices[i], &InverseMatrix);
		m_Frustrum.vertex[i] = v;
	}

	// ���� ���
	m_Frustrum.center.x = (m_Frustrum.vertex[0].x + m_Frustrum.vertex[1].x) / 2;
	m_Frustrum.center.y = (m_Frustrum.vertex[0].y + m_Frustrum.vertex[3].y) / 2;
	m_Frustrum.center.z = (m_Frustrum.vertex[0].z + m_Frustrum.vertex[4].z) / 2;

	// �������� �� ����� ���Ѵ�.
	// �ٱ������� �븻�� �����ϱ� ���� �ð� ��������
	D3DXPlaneFromPoints(&m_Frustrum.plane[0], &m_Frustrum.vertex[0], &m_Frustrum.vertex[1], &m_Frustrum.vertex[2]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[1], &m_Frustrum.vertex[4], &m_Frustrum.vertex[7], &m_Frustrum.vertex[6]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[2], &m_Frustrum.vertex[0], &m_Frustrum.vertex[3], &m_Frustrum.vertex[7]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[3], &m_Frustrum.vertex[1], &m_Frustrum.vertex[5], &m_Frustrum.vertex[6]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[4], &m_Frustrum.vertex[0], &m_Frustrum.vertex[4], &m_Frustrum.vertex[5]);
	D3DXPlaneFromPoints(&m_Frustrum.plane[5], &m_Frustrum.vertex[3], &m_Frustrum.vertex[2], &m_Frustrum.vertex[6]);
}
