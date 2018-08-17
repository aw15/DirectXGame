#pragma once
#include"Util.h"



class SkinMesh
{
public:
	SkinMesh(const std::string& fileName);
	~SkinMesh();
	bool LoadFile();
	void Process();
	void CleanupFbxManager();
	void Optimize();
	//KCYGWREAJVNP
	void ProcessGeometry(FbxNode* inNode);
	void ProcessMesh(FbxNode* inNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessJointsAndAnimations(FbxNode* inNode);
	void ProcessControlPoints(FbxNode* inNode);
	//////////////////////////////////UTILITY//////////////////////////////////////////////////
	unsigned int FindJointIndexUsingName(const std::string& inJointName);
	int FindVertex(const VertexData& inTargetVertex, const std::vector<VertexData>& uniqueVertices);

private:
	FbxManager * mSdkManager =nullptr;
	FbxScene * mScene = nullptr;
	FbxImporter * mImporter = nullptr;

	FbxLongLong mAnimationLength;
	std::string mAnimationName;

	std::string mFileName;
	std::string mWindowMessage;

	std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
	Skeleton mSkeleton;
	bool mHasAnimation;
	unsigned int mTriangleCount;


	std::unordered_map<unsigned int, Parser::Material*> mMaterialLookUp;
public:
	std::vector<VertexData> mVertexData;
	std::vector<Triangle> mTriangles;
};