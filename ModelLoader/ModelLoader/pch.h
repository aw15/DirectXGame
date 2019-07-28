// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

#ifndef PCH_H
#define PCH_H

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<DirectXMath.h>
#include<vector>
#include<unordered_map>
#include<algorithm>

using namespace std;
using namespace DirectX;

struct VertexBoneData
{

	int index;
	float weight;
};



struct SkinnedVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tangentU = { 0,0,0 };
	vector<VertexBoneData> boneData;
	vector<float> BoneWeights;
	vector<int> BoneIndices;


	void PrintPos()
	{
		printf("(%f , %f, %f) ", pos.x, pos.y, pos.z);
	}
};


struct Subset
{
	unsigned int baseVertex;

};


struct Keyframe
{
	Keyframe() = default;
	~Keyframe() = default;

	float TimePos = 0.0f;
	DirectX::XMFLOAT3 Translation{0.f,0.f,0.f};
	DirectX::XMFLOAT3 Scale{ 0.f,0.f,0.f };
	DirectX::XMFLOAT4 RotationQuat{ 0.f,0.f,0.f,0.f };
};

struct BoneAnimation
{
	std::vector<Keyframe> Keyframes;
};

struct AnimationClip
{
	std::vector<BoneAnimation> BoneAnimations;
};


struct Mesh
{
	int baseVertex = 0;
	vector<SkinnedVertex> meshData;
};



class SkinnedData
{
public:
	// Gives parentIndex of ith bone.
	std::vector<int> mBoneHierarchy;

	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;

	AnimationClip mAnimations;
};

#endif //PCH_H
