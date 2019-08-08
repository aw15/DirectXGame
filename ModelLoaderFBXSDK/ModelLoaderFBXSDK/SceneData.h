#pragma once
#include<iostream>
#include<string>
#include<DirectXMath.h>
#include<windows.h>
#include<fbxsdk.h>
#include<vector>
#include <fstream>
#include<string>

using namespace DirectX;
using namespace std;



struct VertexData
{
	XMFLOAT4 position;
	int index[4] = {};
	float weight[4] = {};
};

struct MeshData
{
	vector<VertexData> verticies;
	vector<UINT> indicies;

	struct SubMesh
	{
		SubMesh() : IndexOffset(0), TriangleCount(0) {}

		int IndexOffset;
		int TriangleCount;
	};
};



class VBOMesh
{
public:
	VBOMesh();
	~VBOMesh();

	// Save up data into GPU buffers.
	bool Initialize(const FbxMesh * pMesh);

	void WriteFile(string meshName);

	// Update vertex positions for deformed meshes.
	void UpdateVertexPosition(const FbxMesh * pMesh, const FbxVector4 * pVertices) const;

	// Bind buffers, set vertex arrays, turn on lighting and texture.
	void BeginDraw() const;
	// Draw all the faces with specific material with given shading mode.
	void Draw(int pMaterialIndex) const;
	// Unbind buffers, reset vertex arrays, turn off lighting and texture.
	void EndDraw() const;

	// Get the count of material groups
	int GetSubMeshCount() const { return mSubMeshes.GetCount(); }

private:
	enum
	{
		VERTEX_VBO,
		NORMAL_VBO,
		UV_VBO,
		INDEX_VBO,
		VBO_COUNT,
	};

	// For every material, record the offsets in every VBO and triangle counts
	struct SubMesh
	{
		SubMesh() : IndexOffset(0), TriangleCount(0) {}

		int IndexOffset;
		int TriangleCount;
	};

	UINT mVBONames[VBO_COUNT];
	FbxArray<SubMesh*> mSubMeshes;
	bool mHasNormal;
	bool mHasUV;
	bool mAllByControlPoint; // Save data in VBO by control point or by polygon vertex.


	MeshData meshData;
};