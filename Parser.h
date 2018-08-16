#pragma once
#include<fbxsdk.h>



struct VertexData
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT4 weight;
	int boneIndex[4];
};

class SkinMesh
{
public:
	SkinMesh(const std::string& fileName);
	~SkinMesh();
	bool LoadFile();
	void ProcessGeometry(FbxNode* inNode);
	void ProcessMesh(FbxNode* inNode);
private:
	FbxManager * mSdkManager =nullptr;
	FbxScene * mScene = nullptr;
	FbxImporter * mImporter = nullptr;
	
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxArray<FbxPose*> mPoseArray;

	FbxAnimLayer * mCurrentAnimLayer;

	std::string mFileName;
	std::string mWindowMessage;

	bool mHasAnimation;
	unsigned int mTriangleCount;

	std::vector<VertexData> mVertexData;
	std::vector<UINT> mIndices;
};