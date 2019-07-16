#pragma once
#include"Common/d3dApp.h"
#include"Object.h"
#include"Common/MathHelper.h"
#include"Common/UploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct ObjectConstants
{
	XMFLOAT4X4 worldViewProj = MathHelper::Identity4x4();
};


class MyGame : public D3DApp
{
public:
	MyGame(HINSTANCE hInstance);
	MyGame(const MyGame& rhs) = delete;
	MyGame& operator=(const MyGame& rhs) = delete;
	~MyGame();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPSO();
	void InitMeshData();
	void InitObjects();

	/////������////////
	void RenderObjects();

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f*XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	POINT mLastMousePos;

	std::map<std::string, std::shared_ptr<Mesh>> meshContainer;
	std::vector<std::shared_ptr<Object>> objects;
};

