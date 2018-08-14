#pragma once
#include"Object.h"
#include"Parser.h"
#include "Common/SkinnedData.h"

struct SkinnedModelInstance
{
	SkinnedData* SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	// Called every frame and increments the time position, interpolates the 
	// animations for each bone based on the current animation clip, and 
	// generates the final transforms which are ultimately set to the effect
	// for processing in the vertex shader.
	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		// Loop animation
		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		// Compute the final transforms for this time position.
		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}
};

class Renderer : public D3DApp
{
public:
	Renderer(HINSTANCE hInstance);
	Renderer(const Renderer& rhs) = delete;
	Renderer& operator=(const Renderer& rhs) = delete;
	~Renderer();
	virtual bool Initialize() override;
protected:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	virtual void OnKeyboardInput(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialBuffer(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateSkinnedCBs(const GameTimer& gt);

	void LoadTexture(std::wstring path, std::string name, XMFLOAT3 fersnel = { 0.05f, 0.05f, 0.05f }, XMFLOAT4 albedo = { 1.0f, 1.0f, 1.0f, 1.0f }, float rough = 0.0f);
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();

	void LoadModel(const char* path, std::string name);
	void LoadAnimationModel(const char* path, std::string name);
	void BuildPSOs();
	void BuildFrameResources();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

protected:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mAnimationInputLayout;
	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;
	// Render items divided by PSO.
	std::vector<RenderItem*> mObjectLayer[SORT::count];

	std::unique_ptr<SkinnedModelInstance> mSkinnedModelInst;
	SkinnedData mSkinnedInfo;

	PassConstants mMainPassCB;

	Camera mCamera;

	POINT mLastMousePos;
/////////////////////////////////////////////////////////////////////
	vector<XMFLOAT4X4> boneTransform;
	SkinnedMesh* mMesh;
};
