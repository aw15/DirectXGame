#include "stdafx.h"
#include "Mesh.h"


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

	ColorVertex temp;
	std::vector<ColorVertex> vertices;
	vertices.reserve(size);

	for (int i = 0; i < size; i++)
	{
		in >> temp.Pos.x >> temp.Pos.y >> temp.Pos.z;
		in >> ignore >> ignore >> ignore;
		in >> ignore >> ignore;

		temp.Color = { 0.3, 1, 1 ,1};
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

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(ColorVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);


	ThrowIfFailed(D3DCreateBlob(vbByteSize, &vertexBufferCPU));
	CopyMemory(vertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize,&indexBufferCPU));
	CopyMemory(indexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		commandList.Get(), vertices.data(), vbByteSize, vertexBufferUploader);

	indexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		commandList.Get(), indices.data(), ibByteSize,indexBufferUploader);

	vertexByteStride = sizeof(ColorVertex);
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
