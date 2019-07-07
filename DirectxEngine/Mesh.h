#pragma once

struct ColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};


struct Vertex
{
	XMFLOAT3 Pos = {0,0,0};
	XMFLOAT3 Normal = { 0,1,0 };
	XMFLOAT2 Tex = { 0,0 };
};


struct SubMesh
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	DirectX::BoundingBox Bounds;
};


class Mesh
{
public:
	Mesh();
	~Mesh();
	void LoadMeshData(char* path);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;
	void DisposeUploaders();


public:
	std::string name;

	Microsoft::WRL::ComPtr<ID3D10Blob> vertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> indexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	UINT vertexByteStride=0;
	UINT vertexBufferByteSize=0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize =0;

	std::vector<SubMesh> subMeshArr;
};

