#pragma once

#include "GameObject.h"


class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	float						m_coolTime = 0.0f;
	float						m_invincibilityTime = 1.0f;
	float						m_hp =3;

	CCamera						*m_pCamera = NULL;
	std::vector<CBullet*>        m_bullets;
	CCubeMesh					*pBulletMesh;

	int m_itemCount = 0;


	void SetPosition(float x, float y, float z);
	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);
	void Dead();
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	virtual void Update(float fTimeElapsed=0.016f);
	void Fire();
	void Fire(float speed, XMFLOAT3&);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer();
	virtual ~CAirplanePlayer();
};

class CBoss : public CPlayer
{
public:
	using CPlayer::CPlayer;
	void Update(float fTimeElapsed = 0.016f);
};
