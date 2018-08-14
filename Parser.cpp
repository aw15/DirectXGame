#include"stdafx.h"
#include "Parser.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

void SkinnedMesh::VertexBoneData::AddBoneData(UINT BoneID, float Weight)
{
	for (UINT i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}

SkinnedMesh::SkinnedMesh()
{
	m_NumBones = 0;
	m_pScene = NULL;
}


SkinnedMesh::~SkinnedMesh()
{
}

bool SkinnedMesh::LoadMesh(const string& Filename)
{

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

	if (m_pScene) {
		m_GlobalInverseTransform = XMLoadFloat4x4(&convertXM(m_pScene->mRootNode->mTransformation.Inverse()));

		Ret = InitFromScene(m_pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}



	return Ret;
}


bool SkinnedMesh::InitFromScene(const aiScene* pScene, const string& Filename)
{
	


	UINT NumVertices = 0;
	UINT NumIndices = 0;



	// Initialize the meshes in the scene one by one
	for (UINT i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Bones);
	}

	return true;
}


void SkinnedMesh::InitMesh(UINT MeshIndex,
	const aiMesh* paiMesh,
	vector<VertexBoneData>& Bones)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	LoadBones(MeshIndex, paiMesh, Bones);
}


void SkinnedMesh::LoadBones(UINT MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
	for (UINT i = 0; i < pMesh->mNumBones; i++) {
		UINT BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = convertXM( pMesh->mBones[i]->mOffsetMatrix);
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}
	}
}





void SkinnedMesh::Render()
{

}


UINT SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


UINT SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


UINT SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	UINT PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	UINT NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	UINT ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	UINT NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX& ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	XMMATRIX NodeTransformation =  XMLoadFloat4x4(&convertXM(pNode->mTransformation));

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		XMMATRIX ScalingM = XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		XMFLOAT4 rot{ RotationQ.x,RotationQ.y,RotationQ.z,RotationQ.w };
		
		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMLoadFloat4(&rot));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		XMMATRIX TranslationM = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
		// Combine the above transformations
		
		NodeTransformation = (TranslationM * RotationM * ScalingM);
	}

	XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		UINT BoneIndex = m_BoneMapping[NodeName];
		XMStoreFloat4x4(&m_BoneInfo[BoneIndex].FinalTransformation, m_GlobalInverseTransform * GlobalTransformation * XMLoadFloat4x4(&m_BoneInfo[BoneIndex].BoneOffset));
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<XMFLOAT4X4>& Transforms)
{
	XMMATRIX Identity = XMMatrixIdentity();

	float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	//Transforms.resize(m_NumBones);

	for (UINT i = 0; i < m_NumBones; i++) {
		auto mat = XMMatrixTranspose(XMLoadFloat4x4(&m_BoneInfo[i].FinalTransformation));
		XMStoreFloat4x4(&Transforms[i] ,mat);
	}
}


const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}