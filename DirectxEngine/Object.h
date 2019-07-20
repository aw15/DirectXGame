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


	virtual void Draw(const Mesh* mesh);
	virtual void Updata(const GameTimer& gt);


public:
	UINT ObjCBIndex = 0;
	int NumFramesDirty = gNumFrameResources;
	XMFLOAT4X4 World = MathHelper::Identity4x4();

private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	std::string meshName = "none";

};

