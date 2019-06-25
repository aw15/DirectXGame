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
    
	HINSTANCE AppInst()const;//�������α׷� �ν��Ͻ� �ڵ��� ���纻
	HWND      MainWnd()const;//�� â �ڵ��� ���纻�� �����ִ� �����Լ�
	float     AspectRatio()const;//ȭ�� ��Ⱦ��

    bool Get4xMsaaState()const;//4X MSAA �� Ȱ��ȭ �Ǿ� �ִ��� ����
    void Set4xMsaaState(bool value);//4X MSAA�� ����

	int Run();//�������α׷� �޽��� ����. �޽����� ��� ������ ���� ó���� �� �ֵ��� win32 PeekMessage�� ����Ѵ�.
 
    virtual bool Initialize();//�ڿ��Ҵ�, ��� ��ü �ʱ�ȭ, ���� ���� �� �������α׷��� ������ �ڵ带 ����
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    virtual void CreateRtvAndDsvDescriptorHeaps();//RTV, DSV �����ڸ� �������α׷��� �°� �����Ѵ�. 
	virtual void OnResize(); //â�� ũ�Ⱑ ���ϸ� �ĸ���ۿ� ���̽��ٽ� ���۸� ���� �����Ѵ�.
	virtual void Update(const GameTimer& gt)=0;//�������Ӹ��� ��ü�� ���¸� ������Ʈ�Ѵ�.
    virtual void Draw(const GameTimer& gt)=0;//�� �����Ӹ��� �ĸ���ۿ� ����� �׸���.

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:

	bool InitMainWindow();//win32â �ʱ�ȭ
	bool InitDirect3D();//direct3d �ʱ�ȭ
	void CreateCommandObjects();//��� ��⿭, ��� ��� �Ҵ���, ��� ��� �ϳ��� ����
    void CreateSwapChain();//����ü�� ����

	void FlushCommandQueue();//��ɴ�⿭�� �ִ� ��� ����� ó���� ��ĥ������ CPU�� ��ٸ��� �����.

	ID3D12Resource* CurrentBackBuffer()const;//�ĸ���ۿ� ���� resource�� �����ش�.
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;//���� �ĸ���ۿ� ���� ���� ��� �並 �����ش�.
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;//���� �ĸ���ۿ� ���� ����.���ٽ� �並 �����ش�.

	void CalculateFrameStats();

    void LogAdapters();//��� ���÷��� ����͸� �����Ѵ�.
    void LogAdapterOutputs(IDXGIAdapter* adapter);//�־��� ����Ϳ� ������ ��� ����� �����Ѵ�.
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);//�־��� ��°� �ȼ� ������ ������ �����ϴ� ��� ���÷��� ��带 �����Ѵ�.

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

	// Used to keep track of the �delta-time?and game time (?.4).
	GameTimer mTimer;
	//���丮, ����ü��, ����̽�
    Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	//�ҽ� ��ü, GPU�� Ư���� ����� ó���� ������ CPU�� ��ٸ��� �ϴ� ��
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

