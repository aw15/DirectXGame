// ModelLoader.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <map>
#include<set>
#include<algorithm>
#include<iostream>
string FILENAME = "walking";
#define NUM_BONES_PER_VEREX 96
typedef unsigned int uint;

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

//vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene);
//string determineTextureType(const aiScene* scene, aiMaterial* mat);
//int getTextureIndex(aiString* str);
void processNode(aiNode * node, const aiScene * scene);
void processMesh(aiMesh* mesh, const aiScene* scene);
void AnimNodeProcess();
void MakeBoneInfo(const aiMesh* pMesh);
//string determineTextureType(const aiScene * scene, aiMaterial * mat);

struct VERTEX {
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
	vector<float> weights;
	vector<unsigned int> boneIndices;
};

struct BoneInfo
{
	aiBone*  pBone;
	string boneName;
	int parentIndex = 0;
	int index;
	XMFLOAT4X4 offsetMat;
	XMFLOAT4X4 transformMat;
};

struct Bone
{
	string boneName;
	aiBone* bone;
};

struct AnimationClip
{
	double duration;
	double ticksPerSecond;
	vector<XMFLOAT3> position;
	vector<XMFLOAT4> rotation;
	vector<XMFLOAT3> scale;
};

XMFLOAT4X4 convertXM(const aiMatrix4x4& data)
{
	XMFLOAT4X4 ret;
	ret._11 = data.a1;	ret._12 = data.a2;	ret._13 = data.a3;	ret._14 = data.a4;
	ret._21 = data.b1;	ret._22 = data.b2;	ret._23 = data.b3;	ret._24 = data.b4;
	ret._31 = data.c1;	ret._32 = data.c2;	ret._33 = data.c3;	ret._34 = data.c4;
	ret._41 = data.d1;	ret._42 = data.d2;	ret._43 = data.d3;	ret._44 = data.d4;

	return ret;
}


std::vector<aiNode*> ai_nodes;
std::vector<aiNodeAnim*> ai_nodes_anim;
vector<BoneInfo> boneInfo;
AnimationClip clip;

XMFLOAT4X4 globalInverseTransform;
// Data to fill
vector<VERTEX> vertices;
vector<UINT> indices;
const aiScene* pScene;

UINT numBone=0;

int main()
{
	Assimp::Importer importer;
	importer.GetErrorString();


	pScene = importer.ReadFile(FILENAME+".fbx", ASSIMP_LOAD_FLAGS);


	if (pScene==NULL) {
		printf("FAIL");
		return 0;
	}
	processNode(pScene->mRootNode, pScene);
	AnimNodeProcess();

	globalInverseTransform = convertXM(pScene->mRootNode->mTransformation.Inverse());
	for (UINT i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* mesh = pScene->mMeshes[i];
		numBone += mesh->mNumBones;
		MakeBoneInfo(mesh);
	}




	ofstream out("Model/"+FILENAME+".model");
	out<< vertices.size()<<" " << "VERTEXDATA" << endl;
	for (auto& data : vertices)
	{
		out << data.position.x <<" "<< data.position.y <<" "<< data.position.z<<std::endl;
		out << data.texcoord.x << " " << data.texcoord.y << std::endl;
		out << data.normal.x << " " << data.normal.y << " " << data.normal.z << std::endl;
		out << data.weights[0] << " " << data.weights[1] << " " << data.weights[2] << std::endl;
		out << data.boneIndices[0] << " " << data.boneIndices[1] << " " << data.boneIndices[2] << " " << data.boneIndices[3] << std::endl;
		out << "==========================================================================" << std::endl;

	}
	out << "INDEX" << " " << indices.size() << endl;
	for (auto& data : indices)
	{
		out << data << " ";
	}
	

	out << "BONETREE" <<" "<<numBone<< endl;
	for (auto& data : boneInfo)
	{
		//cout << data.boneName.c_str() << endl;
		out << data.boneName.c_str() << " " << data.index << " " << data.parentIndex << endl;
		auto boneOffset = data.offsetMat.m;
		out << boneOffset[0][0] << " " << boneOffset[0][1] << " " << boneOffset[0][2] << " " << boneOffset[0][3] << endl;
		out << boneOffset[1][0] << " " << boneOffset[1][1] << " " << boneOffset[1][2] << " " << boneOffset[1][3] << endl;
		out << boneOffset[2][0] << " " << boneOffset[2][1] << " " << boneOffset[2][2] << " " << boneOffset[2][3] << endl;
		out << boneOffset[3][0] << " " << boneOffset[3][1] << " " << boneOffset[3][2] << " " << boneOffset[3][3] << endl;
		out << "====================================================================" << endl;
	}
	//cout << "=====================================================" << endl;
	out << "ANIMATIONCLIP" <<" "<<ai_nodes_anim.size()<<endl;
	
	int index = 0;
	for (auto anim : ai_nodes_anim)
	{
		//cout << anim->mNodeName.C_Str() << endl;
		out<<"BONE"<<" "<< index<<" "<< anim->mNumPositionKeys <<endl;
		for (int i=0;i<anim->mNumPositionKeys;i++)
		{
			out	<<	anim->mPositionKeys[i].mTime/15<<endl;
			out <<  anim->mPositionKeys[i].mValue.x<<" "<< anim->mPositionKeys[i].mValue.y<< " " << anim->mPositionKeys[i].mValue.z << endl;
			out <<  anim->mRotationKeys[i].mValue.x << " " << anim->mRotationKeys[i].mValue.y << " " << anim->mRotationKeys[i].mValue.z << " " << anim->mRotationKeys[i].mValue.w << endl;
			out <<  anim->mScalingKeys[i].mValue.x << " " << anim->mScalingKeys[i].mValue.y << " " << anim->mScalingKeys[i].mValue.z << endl;
		}
		index++;
	}




	//ifstream in("Model/" + FILENAME + ".model");

	//string text; 
	//string ignore;
	//int vertexSize = 0;
	//in >> vertexSize;

	//while (!in.eof())
	//{
	//	in >> text;
	//	//if (text == "POSITION")
	//	//{
	//	//	for (int i = 0; i < vertexSize; ++i)
	//	//	{
	//	//		float x, y, z;
	//	//		in >> x >> y >> z;
	//	//		printf("%f %f %f\n", x, y, z);
	//	//	}
	//	//}
	//	//if (text == "UV")
	//	//{
	//	//	for (int i = 0; i < vertexSize; ++i)
	//	//	{
	//	//		float u, v;
	//	//		in >> u >> v;
	//	//		printf("%f %f\n", u, v);
	//	//	}
	//	//}
	//	//if (text == "NORMAL")
	//	//{
	//	//	for (int i = 0; i < vertexSize; ++i)
	//	//	{
	//	//		float x, y, z;
	//	//		in >> x >> y >> z;
	//	//		printf("%f %f %f\n", x, y, z);
	//	//	}
	//	//}
	//	//if (text == "INDEX")
	//	//{
	//	//	int indexSize;
	//	//	in >> indexSize;
	//	//	for (int i = 0; i < indexSize; ++i)
	//	//	{
	//	//		int index;
	//	//		in >> index;
	//	//		printf("%d ", index);
	//	//	}
	//	//}
	//	if (text == "BONETREE")
	//	{
	//		int boneCount=0;
	//		in >> boneCount;
	//		string name;
	//		int index, parentIndex;
	//		float boneOffset[4][4];

	//		for (int i = 0; i < boneCount; i++)
	//		{
	//			in >> name >> index >> parentIndex;
	//			in>>  boneOffset[0][0] >> boneOffset[0][1] >> boneOffset[0][2] >> boneOffset[0][3];
	//			in >> boneOffset[1][0] >> boneOffset[1][1] >> boneOffset[1][2] >> boneOffset[1][3];
	//			in >> boneOffset[2][0] >> boneOffset[2][1] >> boneOffset[2][2] >> boneOffset[2][3];
	//			in >> boneOffset[3][0] >> boneOffset[3][1] >> boneOffset[3][2] >> boneOffset[3][3];
	//			in >> ignore;
	//			//printf("%s\n", name.c_str());
	//			//printf("%f %f %f %f\n", boneOffset[0][0], boneOffset[0][1], boneOffset[0][2], boneOffset[0][3]);
	//			//printf("%f %f %f %f\n", boneOffset[1][0], boneOffset[1][1], boneOffset[1][2], boneOffset[1][3]);
	//			//printf("%f %f %f %f\n", boneOffset[2][0], boneOffset[2][1], boneOffset[2][2], boneOffset[2][3]);
	//			//printf("%f %f %f %f\n", boneOffset[3][0], boneOffset[3][1], boneOffset[3][2], boneOffset[3][3]);
	//			//printf("\n==========================================================\n");
	//		}
	//	}
	//}

	//in.close();
	out.close();

    return 0;
}



void processNode(aiNode * node, const aiScene * scene)
{
	ai_nodes.push_back(node);
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void processMesh(aiMesh * mesh, const aiScene * scene)
{


	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.position.x = (float)mesh->mVertices[i].x;
		vertex.position.y = (float)mesh->mVertices[i].y;
		vertex.position.z = (float)mesh->mVertices[i].z;
		//printf("%F %F %f\n", vertex.X, vertex.Y, vertex.Z);
		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}
		else
		{
			vertex.normal.x = 0;
			vertex.normal.y = 1;
			vertex.normal.z = 0;
		}


		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return;
}


#pragma region 텍스쳐
//ID3D11ShaderResourceView * ModelLoader::getTextureFromModel(const aiScene * scene, int textureindex)
//{
//	HRESULT hr;
//	ID3D11ShaderResourceView *texture;
//
//	int* size = reinterpret_cast<int*>(&scene->mTextures[textureindex]->mWidth);
//
//	hr = CreateWICTextureFromMemory(dev, devcon, reinterpret_cast<unsigned char*>(scene->mTextures[textureindex]->pcData), *size, nullptr, &texture);
//	if (FAILED(hr))
//		MessageBox(hwnd, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);
//
//	return texture;
//}

//string determineTextureType(const aiScene * scene, aiMaterial * mat)
//{
//	aiString textypeStr;
//	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
//	string textypeteststr = textypeStr.C_Str();
//	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5")
//	{
//		if (scene->mTextures[0]->mHeight == 0)
//		{
//			return "embedded compressed texture";
//		}
//		else
//		{
//			return "embedded non-compressed texture";
//		}
//	}
//	if (textypeteststr.find('.') != string::npos)
//	{
//		return "textures are on disk";
//	}
//}
//
//int getTextureIndex(aiString * str)
//{
//	string tistr;
//	tistr = str->C_Str();
//	tistr = tistr.substr(1);
//	return stoi(tistr);
//}
//
//vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName, const aiScene * scene)
//{
//	vector<Texture> textures;
//	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (UINT j = 0; j < textures_loaded.size(); j++)
//		{
//			if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
//			{
//				textures.push_back(textures_loaded[j]);
//				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip)
//		{   // If texture hasn't been loaded already, load it
//			HRESULT hr;
//			Texture texture;
//			int textureindex = getTextureIndex(&str);
//			getTextureFromModel(scene, textureindex);
//			if (textype == "embedded compressed texture")
//			{
//				int textureindex = getTextureIndex(&str);
//				getTextureFromModel(scene, textureindex);
//			}
//			else
//			{
//				string filename = string(str.C_Str());
//				//filename = directory + '/' + filename;
//				//wstring filenamews = wstring(filename.begin(), filename.end());
//		/*		hr = CreateWICTextureFromFile(dev, devcon, filenamews.c_str(), nullptr, &texture.texture);
//				if (FAILED(hr))
//					MessageBox(hwnd, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);*/
//			}
//			texture.type = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//		}
//	}
//	return textures;
//}
//
//void getTextureFromModel(const aiScene * scene, int textureindex)
//{
//	HRESULT hr;
//	
//
//	int* size = reinterpret_cast<int*>(&scene->mTextures[textureindex]->mWidth);
//	scene->mTextures[textureindex]->pcData;
//	//hr = CreateWICTextureFromMemory(dev, devcon, reinterpret_cast<unsigned char*>(scene->mTextures[textureindex]->pcData), *size, nullptr, &texture);
//	/*if (FAILED(hr))
//		MessageBox(hwnd, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);*/
//
//	
//}
#pragma endregion

void AnimNodeProcess()
{
	if (pScene->mNumAnimations == 0)
		return;
	clip.duration = pScene->mAnimations[0]->mDuration;
	clip.ticksPerSecond = pScene->mAnimations[0]->mTicksPerSecond;
	printf("%f %f\n", clip.duration, clip.ticksPerSecond);
	for (int i = 0; i < pScene->mAnimations[0]->mNumChannels; i++)
	{
		ai_nodes_anim.push_back(pScene->mAnimations[0]->mChannels[i]);
	}
}


void MakeBoneInfo(const aiMesh* pMesh)
{
	vector<Bone> boneName;
	//vector<aiBone*> bones;
	for (int i = 0; i < pMesh->mNumBones; i++)
	{
		boneName.push_back({ pMesh->mBones[i]->mName.C_Str(), pMesh->mBones[i]});
	}
	int index = 0;
	string findName;
	auto func = [&findName](Bone& target) {return target.boneName == findName; };
	auto func2 = [&findName](BoneInfo& target) {return target.boneName == findName; };

	for (auto node : ai_nodes)
	{
		findName = node->mName.C_Str();
		auto boneData = find_if(boneName.begin(), boneName.end(), func);
		if (boneData!= boneName.end())
		{
			//printf("name : %s	", node->mName.C_Str());
			//printf("%s\n", node->mParent->mName.C_Str());
			BoneInfo info;
			info.index = index;
			index += 1;
			info.offsetMat = convertXM(boneData->bone->mOffsetMatrix);
			info.transformMat = convertXM(node->mTransformation);
			info.boneName = findName;
			info.pBone = boneData->bone;


			findName = node->mParent->mName.C_Str();
			auto parent = find_if(boneInfo.begin(), boneInfo.end(),func2);
			if (parent != boneInfo.end())
			{
				info.parentIndex = parent->index;
			}
			else
			{
				info.parentIndex = -1;
			}
			boneInfo.push_back(info);
		}
	}
	for (auto data : boneInfo)
	{
		//printf("Index : %d, parent : %d name: %s\n", data.index, data.parentIndex, data.boneName.c_str());
		for (uint j = 0; j < data.pBone->mNumWeights; j++) {
			uint VertexID = data.pBone->mWeights[j].mVertexId;
			float Weight = data.pBone->mWeights[j].mWeight;
			vertices[VertexID].weights.push_back(Weight);
			vertices[VertexID].boneIndices.push_back(data.index);
		}
	}

	for (auto& data : vertices)//무게가 4개가 아닐때 0을 넣어준다.
	{
		if (data.weights.size() < 4)
		{
			while (data.weights.size() != 4)
			{
				data.weights.push_back(0);
			}
		}
		if (data.boneIndices.size() < 4)
		{
			while (data.boneIndices.size() != 4)
			{
				data.boneIndices.push_back(0);
			}
		}
	}
}
