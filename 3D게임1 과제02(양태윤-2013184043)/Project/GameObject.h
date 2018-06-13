#pragma once
#pragma once
#include "Mesh.h"
#include"Camera.h"

class CShader;



class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	XMFLOAT4X4 m_xmf4x4World;
	BoundingOrientedBox			m_StandardOOBB;
	BoundingOrientedBox			m_xmOOBB;
	CGameObject					*m_pObjectCollided = NULL;
	XMFLOAT3					m_xmf3MovingDirection;
	float						m_fMovingSpeed;
	float						m_fMovingRange;

	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;
	XMFLOAT4					m_color;
	float						m_coolTime = 0;
	bool						m_dead = false;
	int							m_tag=ENEMY;
protected:
	CMesh *m_pMesh = NULL;
	CShader *m_pShader = NULL;
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera*);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT
		nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	//게임 객체의 위치를 설정한다. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }
	void SetColor(XMFLOAT4 color) { m_color = color; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);
	//게임 객체를 회전(x-축, y-축, z-축)한다. 
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
public:
	//////////////////////////////////////////////////////////////////////////////
	//void SetTarget(const XMFLOAT3& target) { m_target = target; }
	void Animate(float fTimeElapsed);
};



class CExplosiveObject : public CGameObject
{
public:
	CExplosiveObject(const XMFLOAT3	direction) { m_direction = direction; }
	virtual ~CExplosiveObject() {}
	XMFLOAT3					m_direction;
private:
	float m_totalLifeTime=0;
public:
	//////////////////////////////////////////////////////////////////////////////
	//void SetTarget(const XMFLOAT3& target) { m_target = target; }
	void Animate(float fTimeElapsed);
};

class CBullet : public CGameObject
{
public:
	CBullet(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CBullet();
	void Animate(float fTimeElapsed);
	float m_totalLifeTime = 0;
};