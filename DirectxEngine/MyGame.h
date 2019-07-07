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
	virtual void OnResize() override; //â�� ũ�Ⱑ ���ϸ� �ĸ���ۿ� ���̽��ٽ� ���۸� ���� �����Ѵ�.
	virtual void Update(const GameTimer& gt) override;//�������Ӹ��� ��ü�� ���¸� ������Ʈ�Ѵ�.
	virtual void Draw(const GameTimer& gt) override;//�� �����Ӹ��� �ĸ���ۿ� ����� �׸���.

	void BuildDescriptorHeaps();
	void BuildConstantBuffer();
	void BuildRootSignature();
	void ShaderAndInputLayout();

private:
	Mesh* squareMesh;
	//��� ����
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	//��Ʈ �ñ״�ó
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
};

