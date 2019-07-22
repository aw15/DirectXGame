#pragma once
#include"Mesh.h"



class Object
{
public:
	Object(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,const UINT cbIndex);
	~Object();
	//Mesh
	void SetMeshName(const std::string& name);
	std::string GetMeshName() const;
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
	virtual void Update(const GameTimer& gt);


public:
	UINT ObjCBIndex = 0;
	int NumFramesDirty = gNumFrameResources;
	//XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMFLOAT3 position;
	
	XMFLOAT4X4 world = MathHelper::Identity4x4();
private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	std::string meshName = "none";

};

