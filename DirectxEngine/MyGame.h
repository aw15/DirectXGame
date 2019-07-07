#pragma once
#include"Common/d3dApp.h"
#include"Mesh.h"
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
	virtual ~MyGame();

	virtual bool Initialize() override;


private:
	virtual void OnResize() override; //창의 크기가 변하면 후면버퍼와 깊이스텐실 버퍼를 새로 생성한다.
	virtual void Update(const GameTimer& gt) override;//매프레임마다 객체의 상태를 업데이트한다.
	virtual void Draw(const GameTimer& gt) override;//매 프레임마다 후면버퍼에 장면을 그린다.

	void BuildDescriptorHeaps();
	void BuildConstantBuffer();
	void BuildRootSignature();
	void ShaderAndInputLayout();

private:
	Mesh* squareMesh;
	//상수 버퍼
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	//루트 시그니처
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
};

