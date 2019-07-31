// ModelLodder.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include<fstream>


DirectX::XMFLOAT3 ToXMFloat(aiVector3D& from)
{
	XMFLOAT3 temp;
	temp.x = from.x;
	temp.y = from.y;
	temp.z = from.z;

	return temp;
}

DirectX::XMFLOAT4 ToXMFloat(aiQuaternion& from)
{
	XMFLOAT4 temp;
	temp.x = from.x;
	temp.y = from.y;
	temp.z = from.z;
	temp.w = from.w;

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


std::vector<DirectX::XMFLOAT4X4> boneOffsets;


SkinnedData animationData;


int main()
{
	Assimp::Importer importer;
	string fileName;
	//cin >> fileName;
	fileName = "Model/";
	fileName += "Boxing_Anim.fbx";
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


		animationData.mBoneHierarchy.resize(boneMapping.size());
		animationData.mBoneOffsets.resize(boneMapping.size());

		TravelNode(scene->mRootNode);//트리구조를 내려가며 뼈 계층구조를 만듬.

		SortAndFillBoneData();

		animationData.mAnimations.BoneAnimations.resize(boneMapping.size());
		ReadAnimation(scene);


		ofstream out(fileName + "mesh", ios::trunc);
		out << "VertexSize: ";
		out << verticies.size() << endl;
		for (auto& data : verticies)
		{
			out << data.pos.x << " " << data.pos.y << " " << data.pos.z << endl;
			out << data.normal.x << " " << data.normal.y << " " << data.normal.z << endl;
			out << data.uv.x << " " << data.uv.y << endl;

			out << data.boneData[0].index << " " << data.boneData[1].index << " " << data.boneData[2].index << " " << data.boneData[3].index << endl;
			out << data.boneData[0].weight << " " << data.boneData[1].weight << " " << data.boneData[2].weight << endl;
		}

		out << "IndexSize: " << indices.size() << endl;
		for (int data : indices)
		{
			out << data << endl;
		}
		out.close();


		out.open(fileName + "anim", ios::trunc);
		out << "BoneCount: " << animationData.mBoneHierarchy.size() << endl;
		out << "BoneHierarchy:"<<endl;
		for (int i = 0; i < animationData.mBoneHierarchy.size(); i++)
		{
			out << i <<" "<< animationData.mBoneHierarchy[i]<<endl;
		}
		
		out << "BoneOffset:"<<endl;
		for (auto& arr : animationData.mBoneOffsets)
		{
			out << arr._11 << " " << arr._12 << " " << arr._13 << " " << arr._14 << endl;
			out << arr._21 << " " << arr._22 << " " << arr._23 << " " << arr._24 << endl;
			out << arr._31 << " " << arr._32 << " " << arr._33 << " " << arr._34 << endl;
			out << arr._41 << " " << arr._42 << " " << arr._43 << " " << arr._44 << endl;
		}

		out << "Animation:"<<endl;
		for (auto& bone : animationData.mAnimations.BoneAnimations)
		{
			out << bone.Keyframes.size() << endl;
			for (auto key : bone.Keyframes)
			{
				out << key.Translation.x << " " << key.Translation.y << " " << key.Translation.z << endl;
				out << key.RotationQuat.x << " " << key.RotationQuat.y << " " << key.RotationQuat.z << " "<< key.RotationQuat.w << endl;
				out << key.Scale.x << " " << key.Scale.y << " " << key.Scale.z << endl;
				out << key.TimePos << endl;
			}
			out << "====================================================" << endl;
		}
		out.close();

		ifstream in(fileName + "anim");
		int a, b;
		int boneCount;
		string temp, ignore;
		in >> ignore;
		in >> boneCount;

		
		in >> ignore;
		cout << ignore <<endl;


		for (int i = 0; i < animationData.mBoneHierarchy.size(); i++)
		{
			in >> a >> b;
			cout << a <<" "<< b<<endl;
		}

		in >> ignore;
		XMFLOAT4X4 arr;
		for (int i=0;i<boneCount;i++)
		{
			cout << i << endl;
			in >> arr._11 >> arr._12 >> arr._13 >> arr._14 ;
			in >> arr._21 >> arr._22 >> arr._23 >> arr._24 ;
			in >> arr._31 >> arr._32 >> arr._33 >> arr._34 ;
			in >> arr._41 >> arr._42 >> arr._43 >> arr._44 ;
			Print(arr);
			cout << "====================================================" << endl;
		}

		in >> ignore;
		for (int i = 0; i < boneCount; i++)
		{
			int keyCount;
			in >> keyCount;

			Keyframe key;
			for (int j = 0; j < keyCount; j++)
			{
				in >> key.Translation.x  >> key.Translation.y >> key.Translation.z;
				in >> key.RotationQuat.x >> key.RotationQuat.y >> key.RotationQuat.z>> key.RotationQuat.w;
				in >> key.Scale.x >> key.Scale.y >> key.Scale.z;
				in >> key.TimePos;
			}
			in >> ignore;
		}




		in.close();

		//for (auto& data : boneMapping)
		//{
		//	cout << data.first<<" ";
		//	cout  << data.second<<" ";
		//	cout << animationData.mBoneHierarchy[data.second] << endl;
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
		animationData.mBoneOffsets[boneMapping[node->mName.C_Str()]] = boneOffsets[boneMapping[node->mName.C_Str()]];

		if (boneMapping.find(node->mParent->mName.C_Str()) != boneMapping.end())
		{
			animationData.mBoneHierarchy[boneMapping[node->mName.C_Str()]] = boneMapping[node->mParent->mName.C_Str()];
		}
		else
		{
			animationData.mBoneHierarchy[boneMapping[node->mName.C_Str()]] = -1;
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

		auto channels = anim->mChannels;
		

		for (int j = 0; j < anim->mNumChannels; ++j)
		{
			ReadChannel(channels[j],j);
		}
	}
}

void ReadChannel(aiNodeAnim * animNode, int index)
{
	if (boneMapping.find(animNode->mNodeName.C_Str()) != boneMapping.end())
	{
		int boneIndex = boneMapping[animNode->mNodeName.C_Str()];
		Keyframe frame;
		for (int i = 0; i < animNode->mNumPositionKeys; ++i)
		{
			frame.Translation = ToXMFloat(animNode->mPositionKeys[i].mValue);
			frame.Scale = ToXMFloat(animNode->mScalingKeys[i].mValue);
			frame.RotationQuat = ToXMFloat(animNode->mRotationKeys[i].mValue);


			frame.TimePos = animNode->mPositionKeys[i].mTime;

			animationData.mAnimations.BoneAnimations[boneIndex].Keyframes.push_back(frame);
		}
	}



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