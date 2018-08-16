#pragma once
#include<fbxsdk.h>

class SkinMesh
{
public:
	SkinMesh(const std::string& fileName);
	~SkinMesh();
	bool LoadFile();
	//bool SetCurrentAnimStack(int pIndex);
	//void OnDisplay();
	//void DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
	//	FbxAMatrix& pParentGlobalPosition, FbxPose* pPose);
	//void DrawNode(FbxNode* pNode, FbxTime& pTime,
	//	FbxAnimLayer * pAnimLayer,FbxAMatrix& pParentGlobalPosition,
	//	FbxAMatrix& pGlobalPosition,
	//	FbxPose* pPose);
	//void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
	//	FbxAMatrix& pGlobalPosition, FbxPose* pPose);
private:
	FbxManager * mSdkManager =nullptr;
	FbxScene * mScene = nullptr;
	FbxImporter * mImporter = nullptr;
	
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxArray<FbxPose*> mPoseArray;

	FbxAnimLayer * mCurrentAnimLayer;

	std::string mFileName;
	std::string mWindowMessage;

	mutable FbxTime mFrameTime, mStart, mStop, mCurrentTime;
	mutable FbxTime mCache_Start, mCache_Stop;
};