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
	//��� ���۸� �����Ѵ�.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList);
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
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
	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
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