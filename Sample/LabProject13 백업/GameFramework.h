#pragma once

#define FRAME_BUFFER_WIDTH		640
#define FRAME_BUFFER_HEIGHT		480//윈도우크기는 조정도 못하게 했다. 타깃의 크기를 바꾸게 했다.

#include "Timer.h"
#include "Scene.h"
#include"Camera.h"
#include "Player.h"




class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();
	
    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory;
	IDXGISwapChain3				*m_pdxgiSwapChain;
	ID3D12Device				*m_pd3dDevice;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;//스왑체인 2개로 하겠다
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap;
	UINT						m_nDsvDescriptorIncrementSize;
	//각각용 힙이 2개 필요

	ID3D12CommandAllocator		*m_pd3dCommandAllocator;
	ID3D12CommandQueue			*m_pd3dCommandQueue;
	ID3D12GraphicsCommandList	*m_pd3dCommandList;

	ID3D12Fence					*m_pd3dFence;//CPU,GPU동기화를 위한 FEnce오브젝트
	UINT64						m_nFenceValues[m_nSwapChainBuffers];//스왑체인 개수만큼 만들겠다.
	HANDLE						m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController = NULL;
#endif


	CScene						*m_pScene;//그려야하는 신은 여기에 있다.

	CGameTimer					m_GameTimer;
	_TCHAR						m_pszFrameRate[50];
public:
	CCamera * m_pCamera = NULL;
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다. 
	POINT m_ptOldCursorPos; 
	CPlayer *m_pPlayer = NULL;
};

