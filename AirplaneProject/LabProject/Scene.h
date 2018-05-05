#pragma once

#include "GameObject.h"
#include "Player.h"
#include<chrono>



class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;
	CBoss						*m_pBoss = NULL;
	CGameObject					**m_ppObjects = NULL;
	std::vector<CGameObject*> m_objects;
	CWallsObject				*m_pWallsObject = NULL;
	XMFLOAT3					m_ray = { 0,0,0 };

	void SetRay(const XMFLOAT3& ray) { m_ray = ray;}

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void CheckObjectByObjectCollisions();
	virtual void CheckObjectByWallCollisions();
	virtual void BulletCollisions();
	void Restart();


	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
private:
	float spawnTime = 0;
	float redSpawnTime = 0;
	CCubeMesh *pObjectCubeMesh = nullptr;
	DWORD m_cubeColors[4] = { RGB(0, 255, 0) ,RGB(0, 0, 255) ,RGB(255,0, 255) };
};

