#include"stdafx.h"
#include "Object.h"




Object::Object(ID3D12Device * device, ID3D12GraphicsCommandList * commandList)
{
	this->device = device;
	this->commandList = commandList;
}

Object::~Object()
{
}

void Object::SetMesh(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
}

void Object::Draw()
{
	for (const auto& meshData : mesh->subMeshArr)
	{
		commandList->DrawIndexedInstanced(
			meshData.IndexCount,
			1, 0, 0, 0);
	}
}

void Object::Updata(const GameTimer & gt)
{
}

