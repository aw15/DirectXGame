#pragma once
#include"Mesh.h"



class Object
{
public:
	Object(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,const UINT cbIndex);
	~Object();
	//Mesh
	void SetMesh(const std::string& name,const UINT boneCount = 0);
	std::string GetMesh() const;
	//Transform
	void SetScale(const float x,const float y,const float z);
	void SetRotation(const float x, const float y, const float z);
	void SetPosition(const float x, const float y, const float z);
	XMMATRIX GetFinalTransform();

	void Translation(const float x, const float y, const float z);
	void Roll(const float amount);
	void Pitch(const float amount);
	void Yaw(const float amount);

	virtual void Draw(const Mesh* mesh);
	//Update
	virtual void Update(const GameTimer& gt);
	void UpdateAnimation(const Mesh* mesh, const float deltaTime);

public:
	UINT ObjCBIndex = 0;
	int NumFramesDirty = gNumFrameResources;
	//XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMFLOAT3 position;
	
	XMFLOAT4X4 world = MathHelper::Identity4x4();

	//animation
	float animationCurrentlyTime = 0.0f;
	std::string animationCurrentlyName = "default";
	std::vector<XMFLOAT4X4> animationFinalTransforms;
private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	std::string meshName = "none";

};

