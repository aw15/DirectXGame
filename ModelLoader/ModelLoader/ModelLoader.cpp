// ModelLodder.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include<fstream>


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
void ReadVertex(aiMesh* mesh);
void ReadBoneDataPerVertexAndIndex(aiMesh* mesh, int index);
void TravelNode(aiNode* node);
void SortAndFillBoneData();
//Animation
void ReadAnimation(const aiScene* scene);
void ReadChannel(aiNodeAnim* animNode, int index);

vector<SkinnedVertex> verticies;
vector<int> indices;
vector<Subset> subsets;

unordered_map<string, int> boneMapping;


vector<int> boneHierarchy;

std::vector<DirectX::XMFLOAT4X4> boneOffsets;

AnimationClip animation;




int main()
{
	Assimp::Importer importer;
	string fileName;
	cin >> fileName;
	fileName = "Model/" + fileName;
	//fileName += "Model\\";
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);


	int sum = 0;
	if (scene)
	{
		subsets.resize(scene->mNumMeshes);



		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			if (i != 0)
			{
				subsets[i].baseVertex += subsets[i - 1].baseVertex + scene->mMeshes[i - 1]->mNumVertices;
			}
			else
			{
				subsets[i].baseVertex = 0;
			}

			sum += scene->mMeshes[i]->mNumBones;

			MakeBoneMapping(scene->mMeshes[i]);
			ReadVertex(scene->mMeshes[i]);
		}


	//	cout << sum << "  " << boneMapping.size();

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			ReadBoneDataPerVertexAndIndex(scene->mMeshes[i], i);
		}



		boneHierarchy.resize(boneMapping.size());

		TravelNode(scene->mRootNode);//트리구조를 내려가며 뼈 계층구조를 만듬.

		SortAndFillBoneData();

		ReadAnimation(scene);


		ofstream out(fileName + "mesh", ios::trunc);
		out << "VertexSize: ";
		out << verticies.size() << endl;
		for (auto& data : verticies)
		{
			out << data.pos.x << " " << data.pos.y << " " << data.pos.z << endl;
			out << data.normal.x << " " << data.normal.y << " " << data.normal.z << endl;
			out << data.uv.x << " " << data.uv.y << endl;
		}

		out << "IndexSize: " << indices.size() << endl;
		for (int data : indices)
		{
			out << data << endl;
		}
		out.close();

		//for (int i=0;i<indices.size();i+=3)
		//{
		//	verticies[indices[i]].PrintPos();
		//	verticies[indices[i+1]].PrintPos();
		//	verticies[indices[i+2]].PrintPos();
		//	cout << endl;
		//}

		//for (auto& data : boneMapping)
		//{
		//	cout << data.first<<" ";
		//	cout  << data.second<<" ";
		//	cout << boneHierarchy[data.second] << endl;
		//}


	//	for (auto& data : verticies)
	//	{


	//		cout << "boneIndice : ";
	//		for (auto bone : data.boneData)
	//		{
	//			cout << bone.index << " ";
	//		}
	//		cout << "boneWeight : ";
	//		for (auto bone : data.boneData)
	//		{
	//			cout << bone.weight << " ";
	//		}
	//		cout << endl;
	//	}
	//
	//}

	}
}

void MakeBoneMapping(aiMesh * mesh)
{
	static int num = 0;


	boneOffsets.resize(boneMapping.size() + mesh->mNumBones);

	
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		//cout << mesh->mBones[i]->mName.C_Str()<<endl;
		if (boneMapping.find(mesh->mBones[i]->mName.C_Str()) == boneMapping.end())
		{
			boneOffsets[boneMapping.size()] = ToXMFloatArray( mesh->mBones[i]->mOffsetMatrix );

			boneMapping[mesh->mBones[i]->mName.C_Str()] = boneMapping.size();
		}
	}
}



void ReadVertex(aiMesh* mesh)
{
	SkinnedVertex temp;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		temp.pos = ToXMFloat(mesh->mVertices[i]);
		temp.normal = ToXMFloat(mesh->mNormals[i]);
		temp.uv = ToXMFloat2(mesh->mTextureCoords[0][i]);
		//temp.TangentU = ToXMFloat(mesh->mTangents[i]);


		verticies.push_back(temp);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& Face = mesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		indices.push_back(Face.mIndices[0]);
		indices.push_back(Face.mIndices[1]);
		indices.push_back(Face.mIndices[2]);
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

void SortAndFillBoneData()
{
	for (auto& data : verticies)
	{
		sort(data.boneData.begin(), data.boneData.end(), [](VertexBoneData& a, VertexBoneData& b)
		{
			return a.weight > b.weight;
		});
		while (data.boneData.size() < 4)
		{
			data.boneData.push_back({ 0,0.0f });
		}
	}

}

void ReadAnimation(const aiScene* scene)
{
	int animCount = scene->mNumAnimations;
	auto animArray = scene->mAnimations;
	for (int i = 0; i < animCount; ++i)
	{
		auto anim = animArray[i];
		cout << anim->mName.C_Str() << endl;

		auto channels = anim->mChannels;
		
		animation.BoneAnimations.resize(anim->mNumChannels);

		for (int j = 0; j < anim->mNumChannels; ++j)
		{
			//cout << channels[j]->mNodeName.C_Str()<<endl;
			ReadChannel(channels[j],j);
		}
	}
}

void ReadChannel(aiNodeAnim * animNode, int index)
{
	//if (boneMapping.find(animNode->mNodeName.C_Str()) == boneMapping.end())
	//{
	//	cout << "Not find" << endl;
	//}
	//int boneIndex = boneMapping[animNode->mNodeName.C_Str()];
	//cout << animNode->mNodeName.C_Str() << endl;
}



void ReadBoneDataPerVertexAndIndex(aiMesh* mesh, int index)
{
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			int vertexIndex = subsets[index].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;

			if (vertexIndex >= 0)
			{
				if (boneMapping.find(mesh->mBones[i]->mName.C_Str()) == boneMapping.end())
				{
					cout << "NotFind" << endl;
				}
				verticies[vertexIndex].boneData.push_back({ boneMapping[mesh->mBones[i]->mName.C_Str()] ,mesh->mBones[i]->mWeights[j].mWeight });
				//verticies[vertexIndex].BoneWeights.push_back(mesh->mBones[i]->mWeights[j].mWeight);
				//verticies[vertexIndex].BoneIndices.push_back(boneMapping[mesh->mBones[i]->mName.C_Str()]);
			}
		}
	}
}