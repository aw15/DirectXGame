#include"stdafx.h"
#include "Object.h"




Object::Object(ID3D12Device * device, ID3D12GraphicsCommandList * commandList, const UINT cbIndex)
{
	this->ObjCBIndex = cbIndex;
	this->device = device;
	this->commandList = commandList;

	scale = { 1, 1, 1 };
	rotation = { 0, 0, 0 };
	position = { 0, 0, 0 };
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

void Object::SetScale(const float x, const float y, const float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

}

void Object::SetRotation(const float x, const float y, const float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}

void Object::SetPosition(const float x, const float y, const float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

XMMATRIX Object::GetFinalTransform()
{
	XMMATRIX ret;

	XMVECTOR s = XMLoadFloat3(&scale);
	XMVECTOR r = XMQuaternionRotationMatrix(XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
	XMVECTOR t = XMLoadFloat3(&position);
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	return XMLoadFloat4x4(&world) * XMMatrixAffineTransformation(s,zero,r,t);
}

void Object::Translation(const float x, const float y, const float z)
{

}

void Object::Roll(const float amount)
{


}

void Object::Pitch(const float amount)
{
}

void Object::Yaw(const float amount)
{
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

void Object::Update(const GameTimer & gt)
{
}

