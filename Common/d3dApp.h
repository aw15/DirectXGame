//***************************************************************************************
// d3dApp.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "d3dUtil.h"
#include "GameTimer.h"


class D3DApp
{
protected:

    D3DApp(HINSTANCE hInstance);//
    D3DApp(const D3DApp& rhs) = delete;
    D3DApp& operator=(const D3DApp& rhs) = delete;
    virtual ~D3DApp();

public:

    static D3DApp* GetApp();
    
	HINSTANCE AppInst()const;//응용프로그램 인스턴스 핸들의 복사본
	HWND      MainWnd()const;//주 창 핸들의 복사본을 돌려주는 접근함수
	float     AspectRatio()const;//화면 종횡비

    bool Get4xMsaaState()const;//4X MSAA 가 활성화 되어 있는지 여부
    void Set4xMsaaState(bool value);//4X MSAA를 설정

	int Run();//응용프로그램 메시지 루프. 메시지가 없어도 게임의 논리를 처리할 수 있도록 win32 PeekMessage를 사용한다.
 
    virtual bool Initialize();//자원할당, 장면 물체 초기화, 광원 설정 등 응용프로그램의 고유한 코드를 구현
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    virtual void CreateRtvAndDsvDescriptorHeaps();//RTV, DSV 서술자를 응용프로그램에 맞게 생성한다. 
	virtual void OnResize(); //창의 크기가 변하면 후면버퍼와 깊이스텐실 버퍼를 새로 생성한다.
	virtual void Update(const GameTimer& gt)=0;//매프레임마다 객체의 상태를 업데이트한다.
    virtual void Draw(const GameTimer& gt)=0;//매 프레임마다 후면버퍼에 장면을 그린다.

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:

	bool InitMainWindow();//win32창 초기화
	bool InitDirect3D();//direct3d 초기화
	void CreateCommandObjects();//명령 대기열, 명령 목록 할당자, 명령 목록 하나를 생성
    void CreateSwapChain();//스왑체인 생성

	void FlushCommandQueue();//명령대기열에 있는 모든 명령의 처리를 마칠때까지 CPU가 기다리게 만든다.

	ID3D12Resource* CurrentBackBuffer()const;//후면버퍼에 대한 resource를 돌려준다.
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;//현재 후면버퍼에 대한 렌더 대상 뷰를 돌려준다.
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;//현재 후면버퍼에 대한 깊이.스텐실 뷰를 돌려준다.

	void CalculateFrameStats();

    void LogAdapters();//모든 디스플레이 어댑터를 열거한다.
    void LogAdapterOutputs(IDXGIAdapter* adapter);//주어진 어댑터와 연관된 모든 출력을 열거한다.
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);//주어진 출력과 픽셀 형식의 조합이 지원하는 모든 디스플레이 모드를 나열한다.

protected:

    static D3DApp* mApp;

    HINSTANCE mhAppInst = nullptr; // application instance handle
    HWND      mhMainWnd = nullptr; // main window handle
	bool      mAppPaused = false;  // is the application paused?
	bool      mMinimized = false;  // is the application minimized?
	bool      mMaximized = false;  // is the application maximized?
	bool      mResizing = false;   // are the resize bars being dragged?
    bool      mFullscreenState = false;// fullscreen enabled

	// Set true to use 4X MSAA (?.1.8).  The default is false.
    bool      m4xMsaaState = false;    // 4X MSAA enabled
    UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

	// Used to keep track of the 밺elta-time?and game time (?.4).
	GameTimer mTimer;
	//팩토리, 스왑체인, 디바이스
    Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	//팬스 객체, GPU가 특정한 명령을 처리할 때까지 CPU가 기다리게 하는 것
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    UINT64 mCurrentFence = 0;
	
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    D3D12_VIEWPORT mScreenViewport; 
    D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption = L"d3d App";
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;
};

