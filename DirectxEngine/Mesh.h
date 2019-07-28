#pragma once




struct ColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
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
	bool LoadMeshData(char* path, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList);
	bool LoadAnimData(char* path);

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
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<SubMesh> subMeshArr;
};

