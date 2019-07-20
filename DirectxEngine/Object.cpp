#include"stdafx.h"
#include "Object.h"




Object::Object(ID3D12Device * device, ID3D12GraphicsCommandList * commandList, const UINT cbIndex)
{
	this->ObjCBIndex = cbIndex;
	this->device = device;
	this->commandList = commandList;
}

Object::~Object()
{
}

void Object::SetMeshName(const std::string& name)
{
	meshName = name;
}

std::string Object::GetMeshName() const
{
	return meshName;
}

void Object::Draw(const Mesh* mesh)
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

