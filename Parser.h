#pragma once
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#define ZERO_MEM(a) memset(a, 0, sizeof(a))

using namespace std;



class SkinnedMesh
{
public:
	SkinnedMesh();

	~SkinnedMesh();

	bool LoadMesh(const string& Filename);

	void Render();

	XMFLOAT4X4 convertXM(const aiMatrix4x4& data)
	{
		XMFLOAT4X4 ret;
		ret._11 = data.a1;	ret._12 = data.a2;	ret._13 = data.a3;	ret._14 = data.a4;
		ret._21 = data.b1;	ret._22 = data.b2;	ret._23 = data.b3;	ret._24 = data.b4;
		ret._31 = data.c1;	ret._32 = data.c2;	ret._33 = data.c3;	ret._34 = data.c4;
		ret._41 = data.d1;	ret._42 = data.d2;	ret._43 = data.d3;	ret._44 = data.d4;

		return ret;
	}

	UINT NumBones() const
	{
		return m_NumBones;
	}




	void BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& Transforms);

private:
#define NUM_BONES_PER_VEREX 4

	struct BoneInfo
	{
		XMFLOAT4X4 BoneOffset;
		XMFLOAT4X4 FinalTransformation;

		BoneInfo()
		{
			BoneOffset = MathHelper::Identity4x4();
			FinalTransformation = MathHelper::Identity4x4();
		}
	};

	struct VertexBoneData
	{
		UINT IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(UINT BoneID, float Weight);
	};

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX& ParentTransform);
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	void InitMesh(UINT MeshIndex,
		const aiMesh* paiMesh,
		vector<VertexBoneData>& Bones);
	void LoadBones(UINT MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);


	vector<VertexBoneData> Bones;
	
	std::map<string, UINT> m_BoneMapping; // maps a bone name to its index
	UINT m_NumBones = 0;
	vector<BoneInfo> m_BoneInfo;
	XMMATRIX m_GlobalInverseTransform;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};



