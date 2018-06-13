//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Timer.h"
#include"Shader.h"
#include"Camera.h"
#include"GameObject.h"



class CScene
{
public:
	CScene();
	~CScene();
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	///////////////////////////////////////////
	void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera*);
	void ReleaseUploadBuffers();
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();
	///////////////////////////////////////////////////////////////////////////////
	void SetPlayerAndBoss(CAirplanePlayer *pPlayer, CBoss *pBoss) { m_pPlayer = pPlayer; m_pBoss = pBoss; }
	void SpawnObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void BulletAndObjectCollide(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void Restart();
protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�.
	CInstancingShader * m_pShaders = NULL;
	int m_nShaders = 0;
	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
	/////////////////////////////////////////////////////////////////////////////////////////
	CAirplanePlayer *m_pPlayer = NULL;
	CBoss *m_pBoss = NULL;
	vector<CBullet*>m_bullets;
	vector<CBullet*>m_bossBullets;
	float m_fireTime = 0;
	float m_bossFireTime = 0;
	bool m_setFire = false;
};

