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
	//씬에서 마우스와 키보드 메시지를 처리한다. 
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
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();
	///////////////////////////////////////////////////////////////////////////////
	void SetPlayerAndBoss(CAirplanePlayer *pPlayer, CBoss *pBoss) { m_pPlayer = pPlayer; m_pBoss = pBoss; }
	void SpawnObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void BulletAndObjectCollide(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void Restart();
protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
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

