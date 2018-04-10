//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_hInstance = NULL;
	m_hWnd = NULL;     

    m_hDCFrameBuffer = NULL;   
    m_hBitmapFrameBuffer = NULL;   

	m_bActive = true;    

	m_nObjects = 0;
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
    srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	RECT rc;
	GetClientRect(m_hWnd, &rc);

	m_pCamera = new CCamera();
	m_pCamera->SetViewport(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, 60.0f);
	m_pCamera->SetLookAt(XMFLOAT3(0.0f, 0.0f, -30.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	BuildFrameBuffer(); 

	BuildObjects(); 

	SetupGameState();

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
    HDC hDC = ::GetDC(m_hWnd);

    m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_pCamera->m_Viewport.m_nWidth, m_pCamera->m_Viewport.m_nHeight);    
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
    HBRUSH hBrush = ::CreateSolidBrush(dwColor);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pCamera->m_Viewport.m_xStart, m_pCamera->m_Viewport.m_yStart, m_pCamera->m_Viewport.m_nWidth, m_pCamera->m_Viewport.m_nHeight);
    ::SelectObject(m_hDCFrameBuffer, hOldBrush);
    ::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_pCamera->m_Viewport.m_xStart, m_pCamera->m_Viewport.m_yStart, m_pCamera->m_Viewport.m_nWidth, m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pCamera->m_Viewport.m_xStart, m_pCamera->m_Viewport.m_yStart, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::SetupGameState()
{
}

void CGameFramework::SetupRenderStates()
{
}

void CGameFramework::BuildObjects()
{
	CCubeMesh *pCubeMesh = new CCubeMesh();

	m_nObjects = 4;
	m_pObjects = new CGameObject[m_nObjects];
    m_pObjects[0].SetMesh(pCubeMesh);
	m_pObjects[0].SetColor(RGB(255, 0, 0));
	m_pObjects[0].SetPosition(-13.5f, +2.0f, +14.0f);
	m_pObjects[0].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_pObjects[0].SetRotationSpeed(0.01f);
    m_pObjects[1].SetMesh(pCubeMesh);
	m_pObjects[1].SetColor(RGB(0, 0, 255));
	m_pObjects[1].SetPosition(+13.5f, -2.0f, +14.0f);
	m_pObjects[1].SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	m_pObjects[1].SetRotationSpeed(0.01f);
    m_pObjects[2].SetMesh(pCubeMesh);
	m_pObjects[2].SetColor(RGB(0, 255, 0));
	m_pObjects[2].SetPosition(0.0f, +5.0f, 20.0f);
	m_pObjects[2].SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	m_pObjects[2].SetRotationSpeed(0.01f);
	m_pObjects[2].SetMovingDirection(XMFLOAT3(0.0f, -1.0f, 0.0f));
	m_pObjects[2].SetMovingSpeed(0.002f);
	m_pObjects[2].SetMovingRange(50.0f);
    m_pObjects[3].SetMesh(pCubeMesh);
	m_pObjects[3].SetColor(RGB(0, 255, 255));
	m_pObjects[3].SetPosition(0.0f, 0.0f, 0.0f);
	m_pObjects[3].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_pObjects[3].SetRotationSpeed(0.01f);
	m_pObjects[3].SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pObjects[3].SetMovingSpeed(0.004f);
	m_pObjects[3].SetMovingRange(30.0f);
}

void CGameFramework::ReleaseObjects()
{
	if (m_pObjects) delete [] m_pObjects;
	m_pObjects = NULL;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

    if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput()
{
    if (GetKeyState(VK_LEFT) & 0xFF00) m_pCamera->Move(-0.05f, 0.0f, 0.0f);
    if (GetKeyState(VK_RIGHT) & 0xFF00) m_pCamera->Move(+0.05f, 0.0f, 0.0f);
    if (GetKeyState(VK_UP) & 0xFF00) m_pCamera->Move(0.0f, 0.0f, +0.05f);
    if (GetKeyState(VK_DOWN) & 0xFF00) m_pCamera->Move(0.0f, 0.0f, -0.05f);

	float cxDelta = 0.0f, cyDelta = 0.0f;
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	} 
	if (cxDelta || cyDelta) 
	{
		if (GetKeyState(VK_RBUTTON) & 0xF0)
			m_pCamera->Rotate(cyDelta, 0.0f, -cxDelta);
		else
			m_pCamera->Rotate(cyDelta, cxDelta, 0.0f);        
	} 
}

void CGameFramework::AnimateObjects()
{
	for (int i = 0; i < m_nObjects; i++) m_pObjects[i].Animate();
}

void CGameFramework::FrameAdvance()
{    
    if (!m_bActive) return;

	ProcessInput();

	AnimateObjects();

    ClearFrameBuffer(RGB(255, 255, 255));

	for (int i = 0; i < m_nObjects; i++) m_pObjects[i].Render(m_hDCFrameBuffer, m_pCamera);

	PresentFrameBuffer();
}


