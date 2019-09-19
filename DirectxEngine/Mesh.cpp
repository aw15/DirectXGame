#include "stdafx.h"
#include "Mesh.h"


void Print(const XMFLOAT4X4& mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%5.1f ", mat.m[i][j]);
		}
		std::cout << std::endl;
	}
}

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	DisposeUploaders();
}

bool Mesh::LoadMeshData(char * path, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList)
{
	std::ifstream in(path);

	std::string ignore;
	int size = 0;

	in >> ignore >> size;

	AnimVertex temp;
	std::vector<AnimVertex> vertices;
	vertices.reserve(size);
	
	float floatIgnore;
	int boneIndex[4];
	for (int i = 0; i < size; i++)
	{
		in >> temp.Pos.x >> temp.Pos.y >> temp.Pos.z;// >> floatIgnore;
	//	std::cout<< temp.Pos.x <<" " <<temp.Pos.y <<" "<< temp.Pos.z <<" "<< floatIgnore<<" "<<std::endl;
		in >> ignore >> ignore >> ignore;
		in >> ignore >> ignore;
		in >> boneIndex[0] >> boneIndex[1] >> boneIndex[2] >> boneIndex[3];
		temp.BoneIndices[0] = (BYTE)boneIndex[0];
		temp.BoneIndices[1] = (BYTE)boneIndex[1];
		temp.BoneIndices[2] = (BYTE)boneIndex[2];
		temp.BoneIndices[3] = (BYTE)boneIndex[3];
		//std::cout << boneIndex[0] <<" "<< boneIndex[1] << " " << boneIndex[2] << " " << boneIndex[3]<<std::endl;
		in >> temp.BoneWeights.x >> temp.BoneWeights.y >> temp.BoneWeights.z;
		//std::cout << temp.BoneWeights.x << " " << temp.BoneWeights.y << " " << temp.BoneWeights.z << std::endl;

		temp.Color = { 0.3f, 1.f, 1.f ,1.f};
		vertices.push_back(temp);
	}

	in >> ignore;
	in >> size;


	std::vector<uint16_t> indices;
	indices.reserve(size);
	int index;
	for (int i = 0; i < size; i++)
	{
		in >> index;
		indices.push_back(index);
	}

	in.close();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(AnimVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);


	ThrowIfFailed(D3DCreateBlob(vbByteSize, &vertexBufferCPU));
	CopyMemory(vertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize,&indexBufferCPU));
	CopyMemory(indexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		commandList.Get(), vertices.data(), vbByteSize, vertexBufferUploader);

	indexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		commandList.Get(), indices.data(), ibByteSize,indexBufferUploader);

	vertexByteStride = sizeof(AnimVertex);
	vertexBufferByteSize = vbByteSize;
	indexFormat = DXGI_FORMAT_R16_UINT;
	indexBufferByteSize = ibByteSize;

	SubMesh submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	subMeshArr.push_back(submesh);
}

bool Mesh::LoadAnimData(char * path)
{
	std::ifstream in(path);

	std::string ignore;
	int boneCount;
	in >> ignore;
	in >> boneCount;
	in >> ignore;
	
	animationData.mBoneHierarchy.resize(boneCount);
	//BoneHierarchy Get
	int me, parent;
	for (int i = 0; i < boneCount; ++i)
	{
		in >> me >> parent;
		animationData.mBoneHierarchy[me] = parent;
	}

	in >> ignore;
	animationData.mBoneOffsets.resize(boneCount);
	//Offset Matrix Get
	XMFLOAT4X4 matrix;
	for (int i = 0; i < boneCount; ++i)
	{
		in >> matrix._11 >> matrix._12 >> matrix._13 >> matrix._14;
		in >> matrix._21 >> matrix._22 >> matrix._23 >> matrix._24;
		in >> matrix._31 >> matrix._32 >> matrix._33 >> matrix._34;
		in >> matrix._41 >> matrix._42 >> matrix._43 >> matrix._44;
		//Print(matrix);
		animationData.mBoneOffsets[i] = matrix;
	}
	int animationLen;

	XMFLOAT3 translation;
	XMFLOAT4 rotation;
	XMFLOAT3 scale;
	float time;

	AnimationClip clip;
	clip.BoneAnimations.resize(boneCount);
	

	for (int i = 0; i < boneCount; ++i)
	{
		in >> ignore;
	//	std::cout << ignore << std::endl;
		in >> animationLen;
		clip.BoneAnimations[i].Keyframes.resize(animationLen);
		for (int j = 0; j < animationLen; ++j)
		{
			in >> translation.x >> translation.y >> translation.z;
			in >> rotation.x >> rotation.y >> rotation.z>> rotation.w;
			in >> scale.x >> scale.y >> scale.z;
			in >> time;
			//std::cout << time<<std::endl;

			clip.BoneAnimations[i].Keyframes[j].Translation = translation;
			clip.BoneAnimations[i].Keyframes[j].RotationQuat = rotation;
			clip.BoneAnimations[i].Keyframes[j].Scale = scale;
			clip.BoneAnimations[i].Keyframes[j].TimePos = time;
		}
	}

	in.close();


	animationData.mAnimations["default"] = clip;

	//std::cout << animationData.mAnimations["default"].BoneAnimations[0].Keyframes[12].TimePos << std::endl;

	return false;
}

D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = vertexByteStride;
	vbv.SizeInBytes = vertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
	ibv.SizeInBytes = indexBufferByteSize;
	ibv.Format = indexFormat;

	return ibv;
}

void Mesh::DisposeUploaders()
{
	vertexBufferUploader = nullptr;
	indexBufferUploader = nullptr;
}
