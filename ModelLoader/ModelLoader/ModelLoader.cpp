// ModelLodder.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"



// TODO: 여기에 미리 컴파일하려는 헤더 추가
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

XMFLOAT4X4 ToXMFloatArray(aiMatrix4x4& matrix)
{
	XMFLOAT4X4 ret;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.m[i][j] = matrix[i][j];
		}
	}

	return ret;
}

void Print(const XMFLOAT4X4& mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%5.1f ", mat.m[i][j]);
		}
		cout << endl;
	}
}


void MakeBoneMapping(aiMesh* mesh);
void TravelMesh(aiMesh* mesh, int index);
void TravelNode(aiNode* node);

vector<Mesh> meshes;
unordered_map<string, int> boneMapping;


vector<int> boneHierarchy;

std::vector<DirectX::XMFLOAT4X4> boneOffsets;

//std::unordered_map<std::string, AnimationClip> mAnimations;


int main()
{
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


		//for (auto& data:boneMapping)
		//{
		//	cout << data.first << " " << data.second << endl;
		//}

		boneHierarchy.resize(boneMapping.size());
				
		TravelNode(scene->mRootNode);//트리구조를 내려가며 뼈 계층구조를 만듬.


		for (auto& data : boneMapping)
		{
			cout << data.first<<" ";
			cout  << data.second<<" ";
			cout << boneHierarchy[data.second] << endl;
		}


		//for (int i=0;i< boneHierarchy.size();i++)
		//{
		//	cout << i << " " << boneHierarchy[i] << endl;

		//	//Print(boneOffsets[i]);
		//}


		

		//for (int i = 0; i < scene->mNumMeshes; i++)
		//{
		//	for (auto data : meshes[i].meshData)
		//	{
		//		cout << "boneIndice : ";
		//		for (auto bone : data.BoneIndices)
		//		{
		//			cout << bone << " ";
		//		}
		//		cout << "boneWeight : ";
		//		for (auto weight : data.BoneWeights)
		//		{
		//			cout << weight << " ";
		//		}
		//		cout << endl;
		//	}
		//}
	}

}



void MakeBoneMapping(aiMesh * mesh)
{
	static int num = 0;


	boneOffsets.resize(boneMapping.size() + mesh->mNumBones);
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		if (boneMapping.find(mesh->mBones[i]->mName.C_Str()) == boneMapping.end())
		{
			boneOffsets[boneMapping.size()] = ToXMFloatArray( mesh->mBones[i]->mOffsetMatrix );

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
	if (node->mParent && boneMapping.find(node->mName.C_Str()) != boneMapping.end())
	{
		if (boneMapping.find(node->mParent->mName.C_Str()) != boneMapping.end())
		{
			boneHierarchy[boneMapping[node->mName.C_Str()]] = boneMapping[node->mParent->mName.C_Str()];
		}
		else
		{
			boneHierarchy[boneMapping[node->mName.C_Str()]] = -1;
		}
	}
	

	for (int i = 0; i < node->mNumChildren; i++)
	{
		TravelNode(node->mChildren[i]);
	}
}