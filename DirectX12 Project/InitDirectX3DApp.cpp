#include<DirectXColors.h>
#include"common\d3dApp.h"
#include"common/MathHelper.h"
#include"common/UploadBuffer.h"
using namespace DirectX;
using Microsoft::WRL::ComPtr;
#pragma region 챕터3 초기화 클래스
class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();
	virtual bool Initialize() override;
public:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;
};

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) :D3DApp(hInstance)
{
}

InitDirect3DApp::~InitDirect3DApp()
{
}

bool InitDirect3DApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;
	return true;
}

void InitDirect3DApp::OnResize()
{
	D3DApp::OnResize();
}

void InitDirect3DApp::Update(const GameTimer& gt)
{

}

void InitDirect3DApp::Draw(const GameTimer& gt)
{
	//재설정은 GPU가 관련 명령 목록들을 모두 처리한 후에 일어난다.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	//명령 목록을 ExecuteCommandList를 통해서 명령 대기열에 추가했다면
	//명령목록을 재설정 할 수 있다.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	//후면버퍼를 렌더타깃 상태로 만들어둔다.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//뷰포트와 시저직사각형을 설정한다.
	//명령목록 재설정할 때마다 이들도 재설정해야 한다.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
	//후면버퍼와 깊이버퍼를 지운다.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	//렌더링 결과가 기록될 렌더 대상 버퍼들을 지정한다.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	//후면버퍼를 present상태로 변경한다.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();

}
#pragma endregion

struct PosData
{
	XMFLOAT3 Pos;
};
struct ColorData
{
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	float gTime;
};

class BoxApp :public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance);
	BoxApp(const BoxApp& rhs) = delete;
	BoxApp& operator=(const BoxApp& rhs) = delete;
	~BoxApp();

	virtual bool Initialize() override;
private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;
	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();//정점 버퍼 생성 및 값 전달
	void BuildPSO();
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	//상수버퍼
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>>  mObjectCB = nullptr;
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12PipelineState> mPSO = nullptr;
	POINT mLastMousePos;
	float mTheta = 1.5*XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
	//색깔용 버퍼들
	Microsoft::WRL::ComPtr<ID3DBlob> colorBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> colorBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> colorBufferUploader = nullptr;
	D3D12_VERTEX_BUFFER_VIEW colorBufferView;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	try
	{
		BoxApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}



void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState&MK_LBUTTON) != 0)//회전
	{
		//마우스 한 픽셀 이동을 4분의 1도에 대응시킨다.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));
		//입력에 기초에 각도를 갱신해서 카메라가 상자를 중심으로 공전하게 한다.
		mTheta += dx;
		mPhi += dy;
		//Phi각도를 제한한다.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)//확대
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void BoxApp::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapdesc;
	cbvHeapdesc.NumDescriptors = 1;
	cbvHeapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapdesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&cbvHeapdesc, IID_PPV_ARGS(&mCbvHeap))
	);
}

void BoxApp::BuildConstantBuffers()
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);     

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();

	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	md3dDevice->CreateConstantBufferView(&cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void BoxApp::BuildRootSignature()
{
	//쉐이더 입력 레지스터들이 받아야 하는 자원들을 정의하는 것.
	//상수버퍼가 몇개인지, 텍스쳐, 샘플러가 몇개인지
	//루트 매개변수 -> 루트 시그니쳐
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];
	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 
		1	//테이블의 서술자 개수
		, 0//이 루트 매개변수에 묶일 셰이더 인수들의 기준 레지스터번호.
	);
	slotRootParameter[0].InitAsDescriptorTable(1 //테이블의 서술자 개수
		, &cbvTable//구간들의 배열을 가리키는 포인터
	);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void BoxApp::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	mvsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout = 
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA}
		,{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA}
	};
}

void BoxApp::BuildBoxGeometry()
{
	std::array<PosData, 9> vertices =
	{
		PosData{XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		PosData{ XMFLOAT3(-1.0f, +1.0f, -1.0f)},
		PosData{ XMFLOAT3(+1.0f, +1.0f, -1.0f)},
		PosData{ XMFLOAT3(+1.0f, -1.0f, -1.0f)},
		PosData{ XMFLOAT3(-1.0f, -1.0f, +1.0f)},
		PosData{ XMFLOAT3(-1.0f, +1.0f, +1.0f)},
		PosData{ XMFLOAT3(+1.0f, +1.0f, +1.0f)},
		PosData{ XMFLOAT3(+1.0f, -1.0f, +1.0f)},
		//피라미드
		PosData{ XMFLOAT3(+0.0f, 1.0f, 0.0f)}
	};
	std::array<ColorData,9> colors = 
	{
		ColorData{XMFLOAT4(Colors::White) },
		ColorData{XMFLOAT4(Colors::Black)},
		ColorData{XMFLOAT4(Colors::Red)},
		ColorData{XMFLOAT4(Colors::Green)},
		ColorData{XMFLOAT4(Colors::Blue) },
		ColorData{XMFLOAT4(Colors::Yellow)},
		ColorData{XMFLOAT4(Colors::Cyan) },
		ColorData{XMFLOAT4(Colors::Magenta)},
		//피라미드
		ColorData{ XMFLOAT4(Colors::White) }
	};

	std::array<std::uint16_t, 54> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7,
		//피라미드
		//밑바닥
		4, 0, 3,
		4, 3, 7,
		//왼쪽
		8,0,4,
		////뒤쪽
		8,3,0,
		////오른쪽
		8,7,3
		////앞쪽
		,8,4,7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(PosData);
	const UINT colorBufferSize = (UINT)colors.size() * sizeof(ColorData);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
	CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
	CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

	colorBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), colors.data(), colorBufferSize, colorBufferUploader);//색깔용 버퍼만들기
	//색깔용 버퍼 뷰 만들기
	
	colorBufferView.BufferLocation = colorBufferGPU->GetGPUVirtualAddress();
	colorBufferView.StrideInBytes = sizeof(ColorData);
	colorBufferView.SizeInBytes = colorBufferSize;



	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);
	//정점버퍼 뷰와 인덱스버퍼 뷰를 만드는 데 사용.
	mBoxGeo->VertexByteStride = sizeof(PosData);//한 정점데이터의 크기
	mBoxGeo->VertexBufferByteSize = vbByteSize;//전체 정점 데이터의 크기
	mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	mBoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	mBoxGeo->DrawArgs["box"] = submesh;
}

void BoxApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

BoxApp::BoxApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

BoxApp::~BoxApp()
{
}

bool BoxApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void BoxApp::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void BoxApp::Update(const GameTimer & gt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
	
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view*proj;

	//상수 버퍼 업데이트
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));//쉐이더에서는 열행렬이므로 전치해서 보낸다.
	objConstants.gTime = gt.TotalTime();
	mObjectCB->CopyData(0, objConstants);
}

void BoxApp::Draw(const GameTimer & gt)
{
	//재설정은 GPU가 관련 명령 목록들을 모두 처리한 후에 일어난다.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	//명령 목록을 ExecuteCommandList를 통해서 명령 대기열에 추가했다면
	//명령목록을 재설정 할 수 있다.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));
	//후면버퍼를 렌더타깃 상태로 만들어둔다.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//뷰포트와 시저직사각형을 설정한다.
	//명령목록 재설정할 때마다 이들도 재설정해야 한다.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
	//후면버퍼와 깊이버퍼를 지운다.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	//렌더링 결과가 기록될 렌더 대상 버퍼들을 지정한다.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
	mCommandList->IASetVertexBuffers(1, 1, &colorBufferView);
	mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//루트 시그니처 테이블에 상수버퍼 서술자를 연결함
	mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

	mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

	mCommandList->DrawIndexedInstanced(
		12+6,//mBoxGeo->DrawArgs["box"].IndexCount,
		1, 36, 0, 0);

	//후면버퍼를 present상태로 변경한다.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}
