#pragma once
#include "common/d3dUtil.h"
#include "common/MathHelper.h"
#include "common/UploadBuffer.h"


struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0,0.0,0.0 };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f,0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f,0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};


class FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	//명령 할당자는 GPU가 명령들을 다 처리한 후 재설정해야 한다.
	//따라서 프레임마다 할당자가 필요하다.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;
	//상수버퍼는 그것을 참조하는 명령들을 GPU가 다 처리한 후에 갱신해야 한다.
	//따라서 프레임마다 상수버퍼를 새로 만들어야 한다.
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>>ObjectCB = nullptr;
	//Fence는 현재 울타리 지점까지의 명령들을 표시하는 값이다.
	//이값은 GPU가 아직 이프레임 자원들을 사용하고 있는지 판정하는 용도로 쓰인다.
	UINT64 Fence = 0;
};



