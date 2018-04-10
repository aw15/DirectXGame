#include "StdAfx.h"
#include "GameObject.h"

CPolygon::CPolygon(int nVertices) 
{ 
	if (nVertices > 0)
	{
		m_nVertices = nVertices; 
		m_pVertices = new CVertex[nVertices]; 
	}
}

CPolygon::~CPolygon() 
{ 
	if (m_pVertices) delete [] m_pVertices; 
}

void CPolygon::SetVertex(int nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

void CPolygon::Draw(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera *pCamera)
{
	CVertex vertex;
    XMFLOAT3 xmf3Previous, xmf3Current;
    for (int i = 0; i <= m_nVertices; i++) 
    {
		vertex = m_pVertices[i % m_nVertices];
//World/View/Projection Transformation(Perspective Divide)
//		xmf4Current = Vector3::Transform(vertex.m_xmf3Position, xm4x4Transform);
//		if (xmf4Current.z != 0.0f) { xmf4Current.x = xmf4Current.x / xmf4Current.w; xmf4Current.y = xmf4Current.y / xmf4Current.w; xmf4Current.z = xmf4Current.z / xmf4Current.w; }
		xmf3Current = Vector3::TransformCoord(vertex.m_xmf3Position, xm4x4Transform);
//		if ((xmf3Current.z < 0.0f) || (xmf3Current.z > 1.0f)) continue;
//Screen Transformation
        xmf3Current.x = +xmf3Current.x * (pCamera->m_Viewport.m_nWidth * 0.5f) + pCamera->m_Viewport.m_xStart + (pCamera->m_Viewport.m_nWidth * 0.5f);
        xmf3Current.y = -xmf3Current.y * (pCamera->m_Viewport.m_nHeight * 0.5f) + pCamera->m_Viewport.m_yStart + (pCamera->m_Viewport.m_nHeight * 0.5f);

        if (i != 0) 
		{
			::MoveToEx(hDCFrameBuffer, (long)xmf3Previous.x, (long)xmf3Previous.y, NULL);
			::LineTo(hDCFrameBuffer, (long)xmf3Current.x, (long)xmf3Current.y);
		}
        xmf3Previous = xmf3Current; 
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(int nPolygons)
{
	if (nPolygons > 0)
	{
		m_nPolygons = nPolygons;
		m_ppPolygons = new CPolygon * [nPolygons];
		m_nReferences = 0;
	}
}

CMesh::~CMesh(void)
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete [] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon *pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons) && m_ppPolygons)
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera *pCamera)
{
    for (int j = 0; j < m_nPolygons; j++)
    {
        m_ppPolygons[j]->Draw(hDCFrameBuffer, xm4x4Transform, pCamera);
    } 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh() : CMesh(6)
{
	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-2, +2, -2));
	pFrontFace->SetVertex(1, CVertex(+2, +2, -2));
	pFrontFace->SetVertex(2, CVertex(+2, -2, -2));
	pFrontFace->SetVertex(3, CVertex(-2, -2, -2));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-2, +2, +2));
	pTopFace->SetVertex(1, CVertex(+2, +2, +2));
	pTopFace->SetVertex(2, CVertex(+2, +2, -2));
	pTopFace->SetVertex(3, CVertex(-2, +2, -2));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-2, -2, +2));
	pBackFace->SetVertex(1, CVertex(+2, -2, +2));
	pBackFace->SetVertex(2, CVertex(+2, +2, +2));
	pBackFace->SetVertex(3, CVertex(-2, +2, +2));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-2, -2, -2));
	pBottomFace->SetVertex(1, CVertex(+2, -2, -2));
	pBottomFace->SetVertex(2, CVertex(+2, -2, +2));
	pBottomFace->SetVertex(3, CVertex(-2, -2, +2));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-2, +2, +2));
	pLeftFace->SetVertex(1, CVertex(-2, +2, -2));
	pLeftFace->SetVertex(2, CVertex(-2, -2, -2));
	pLeftFace->SetVertex(3, CVertex(-2, -2, +2));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+2, +2, -2));
	pRightFace->SetVertex(1, CVertex(+2, +2, +2));
	pRightFace->SetVertex(2, CVertex(+2, -2, +2));
	pRightFace->SetVertex(3, CVertex(+2, -2, -2));
	SetPolygon(5, pRightFace);
}

CCubeMesh::~CCubeMesh(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject() 
{ 
	m_pMesh = NULL; 
	m_xmf4x4World = Matrix4x4::Identity();

	m_dwColor = RGB(0, 0, 0);

	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject()
{
	m_pMesh = pMesh; 
}

CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z) 
{
	m_xmf4x4World._41 = x; 
	m_xmf4x4World._42 = y; 
	m_xmf4x4World._43 = z; 
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position) 
{ 
	m_xmf4x4World._41 = xmf3Position.x; 
	m_xmf4x4World._42 = xmf3Position.y; 
	m_xmf4x4World._43 = xmf3Position.z; 
}

XMFLOAT3 CGameObject::GetPosition() 
{ 
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 
}

XMFLOAT3 CGameObject::GetLook() 
{ 	
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CGameObject::GetUp() 
{ 	
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{ 	
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fYaw, fPitch, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::Animate()
{
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed);
	if (m_fMovingSpeed != 0.0f)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		float fDistance = Vector3::Length(xmf3Position);
		if (fDistance >= m_fMovingRange) m_xmf3MovingDirection = Vector3::ScalarProduct(m_xmf3MovingDirection, -1.0f);
		Move(m_xmf3MovingDirection, m_fMovingSpeed);
	}
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	XMFLOAT4X4 xm4x4Transform = Matrix4x4::Multiply(m_xmf4x4World, pCamera->m_xmf4x4ViewProject);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer, xm4x4Transform, pCamera);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera() 
{ 
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f); 
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f); 
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f); 

	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_xmf4x4ViewProject = Matrix4x4::Identity();

    m_Viewport.m_xStart = 0;
    m_Viewport.m_yStart = 0;
    m_Viewport.m_nWidth = 640;
    m_Viewport.m_nHeight = 480;
}

CCamera::~CCamera() 
{
}

void CCamera::GenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
    m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
    m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
    m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
    m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
    m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
    m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);

	m_xmf4x4ViewProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4Projection);
}

void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33));

	GenerateViewMatrix();
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight)
{
    m_Viewport.m_xStart = xTopLeft;
    m_Viewport.m_yStart = yTopLeft;
    m_Viewport.m_nWidth = nWidth;
    m_Viewport.m_nHeight = nHeight;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle)
{
	float fAspectRatio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight));
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(fFOVAngle, fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::Move(XMFLOAT3& xmf3Shift) 
{ 
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	GenerateViewMatrix();
}

void CCamera::Move(float x, float y, float z) 
{ 
	Move(XMFLOAT3(x, y, z)); 
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
    if (fPitch != 0.0f)
    {
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
    } 
    if (fYaw != 0.0f)
    {
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
    } 
    if (fRoll != 0.0f)
    {
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
    } 

	GenerateViewMatrix();
}

