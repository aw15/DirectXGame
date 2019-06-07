// ModelLodder.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<DirectXMath.h>
#include<vector>
#include<unordered_map>


using namespace std;
using namespace DirectX;

DirectX::XMFLOAT3 ToXMFloat(aiVector3D& from)
{
	XMFLOAT3 temp;
	temp.x = from.x;
	temp.y = from.y;
	temp.z = from.z;

	return temp;
}


DirectX::XMFLOAT2 ToXMFloat(aiVector2D& from)
{
	XMFLOAT2 temp;
	temp.x = from.x;
	temp.y = from.y;

	return temp;
}

DirectX::XMFLOAT2 ToXMFloat2(aiVector3D& from)
{
	XMFLOAT2 temp;
	temp.x = from.x;
	temp.y = from.y;

	return temp;
}
struct SkinnedVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
	DirectX::XMFLOAT3 TangentU = {0,0,0};
	vector<float> BoneWeights;
	vector<int> BoneIndices;
};

struct Mesh
{
	int baseVertex=0;
	vector<SkinnedVertex> meshData;
};



void MakeBoneMapping(aiMesh* mesh);
void TravelMesh(aiMesh* mesh, int index);
void TravelNode(aiNode* node);

vector<Mesh> meshes;
unordered_map<string, int> boneMapping;


int main()
{
    std::cout << "Hello World!\n"; 
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile("test.fbx", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);



	if (scene)
	{
		meshes.resize(scene->mNumMeshes);


		
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			if (i != 0)
			{
				meshes[i].baseVertex += meshes[i-1].baseVertex + scene->mMeshes[i-1]->mNumVertices;
			}
			else
			{
				meshes[i].baseVertex = 0;
			}

			MakeBoneMapping(scene->mMeshes[i]);
		}


		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			TravelMesh(scene->mMeshes[i], i);

		}


		for (auto& data:boneMapping)
		{
			cout << data.first << " " << data.second << endl;
		}

		//TravelNode(scene->mRootNode);


		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			for (auto data : meshes[i].meshData)
			{
				cout << "boneIndice : ";
				for (auto bone : data.BoneIndices)
				{
					cout << bone << " ";
				}
				cout << "boneWeight : ";
				for (auto weight : data.BoneWeights)
				{
					cout << weight << " ";
				}
				cout << endl;
			}
		}
	}

	//cout << "test";
}



void MakeBoneMapping(aiMesh * mesh)
{
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		if (boneMapping.find(mesh->mBones[i]->mName.C_Str()) == boneMapping.end())
		{
			boneMapping[mesh->mBones[i]->mName.C_Str()] = boneMapping.size();
		}
	}
}

void TravelMesh(aiMesh* mesh,int index)
{
	SkinnedVertex temp;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		temp.Pos = ToXMFloat(mesh->mVertices[i]);
		temp.Normal = ToXMFloat(mesh->mNormals[i]);
		temp.TexC = ToXMFloat2(mesh->mTextureCoords[0][i]);
		//temp.TangentU = ToXMFloat(mesh->mTangents[i]);


		meshes[index].meshData.push_back(temp);
	}


	for (int i = 0; i < mesh->mNumBones; i++)
	{
		for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{	
			int vertexIndex = meshes[index].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;

			//cout << meshes[index].meshData[vertexIndex].Pos.x << endl;

			if (vertexIndex >= 0 && vertexIndex < meshes[index].meshData.size())
			{
				//cout << boneMapping[mesh->mBones[i]->mName.C_Str()] << " " << mesh->mBones[i]->mWeights[j].mWeight << endl;
				meshes[index].meshData[vertexIndex].BoneWeights.push_back(mesh->mBones[i]->mWeights[j].mWeight);
				meshes[index].meshData[vertexIndex].BoneIndices.push_back(boneMapping[mesh->mBones[i]->mName.C_Str()]);
			}
		}

	}

}


void TravelNode(aiNode* node)
{
	if (node->mParent)
	{
		cout << node->mName.C_Str() << " " << node->mParent->mName.C_Str() << endl;
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		TravelNode(node->mChildren[i]);
	}
}