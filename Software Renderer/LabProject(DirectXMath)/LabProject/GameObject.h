#pragma once

#define DegreeToRadian(x)	((x)*3.1415926535/180.0)

struct VIEWPORT
{
	int						m_xStart;
	int						m_yStart;
	int						m_nWidth;
	int						m_nHeight;
};

class CCamera
{
public:
	CCamera();
	~CCamera();

	XMFLOAT3				m_xmf3Position;             
    XMFLOAT3				m_xmf3Right;             
    XMFLOAT3				m_xmf3Up;             
    XMFLOAT3				m_xmf3Look;   

	XMFLOAT4X4				m_xmf4x4View;         	
    XMFLOAT4X4				m_xmf4x4Projection;   
	XMFLOAT4X4				m_xmf4x4ViewProject;

	VIEWPORT				m_Viewport;

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle);
	void SetLookAt(XMFLOAT3& vPosition, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up=XMFLOAT3(0.0f, 1.0f, 0.0f));
	void SetViewport(int xStart, int yStart, int nWidth, int nHeight);

	void Move(XMFLOAT3& xmf3Shift);
    void Move(float x, float y, float z);
	void Rotate(float fPitch=0.0f, float fYaw=0.0f, float fRoll=0.0f);
};

class CVertex
{
public:
	CVertex() { }
    CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }

    XMFLOAT3				m_xmf3Position;  
};

class CPolygon
{
public:
	CPolygon(int nVertices);
	virtual ~CPolygon();

    int						m_nVertices;     
    CVertex					*m_pVertices;   

	void SetVertex(int nIndex, CVertex& vertex);
	void Draw(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera *pCamera);
};

class CMesh
{
public:
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int						m_nReferences;

    int						m_nPolygons;     
    CPolygon				**m_ppPolygons;   

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

public:
	void SetPolygon(int nIndex, CPolygon *pPolygon);
	virtual void Render(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera *pCamera);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh();
	virtual ~CCubeMesh();
};

class CGameObject
{
public:
	CGameObject(CMesh *pMesh);
	CGameObject();
	~CGameObject();

public:
    CMesh					*m_pMesh; 
    XMFLOAT4X4				m_xmf4x4World;             

	DWORD					m_dwColor;

	XMFLOAT3				m_xmf3MovingDirection;
	float					m_fMovingSpeed;
	float					m_fMovingRange;

	XMFLOAT3				m_xmf3RotationAxis;
	float					m_fRotationSpeed;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance=1.0f);
	void MoveUp(float fDistance=1.0f);
	void MoveForward(float fDistance=1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	void Rotate(float fPitch=10.0f, float fYaw=10.0f, float fRoll=10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void Animate();
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

