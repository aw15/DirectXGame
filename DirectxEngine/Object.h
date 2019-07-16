#pragma once
#include "Mesh.h"




class Object
{
public:
	Object(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~Object();
	
	void SetMesh(std::shared_ptr<Mesh> mesh);

	virtual void Draw();
	virtual void Updata(const GameTimer& gt);

private:
	std::shared_ptr<Mesh> mesh;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
};

