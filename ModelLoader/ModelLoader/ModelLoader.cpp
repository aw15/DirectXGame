// ModelLoader.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <map>
#include<set>
#include<algorithm>
#include<iostream>
string FILENAME = "walking";
#define NUM_BONES_PER_VEREX 96
typedef unsigned int UINT;

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

//vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene);
//string determineTextureType(const aiScene* scene, aiMaterial* mat);
//int getTextureIndex(aiString* str);
void processNode(aiNode * node, const aiScene * scene);
void processMesh(aiMesh* mesh, const aiScene* scene);
//string determineTextureType(const aiScene * scene, aiMaterial * mat);

struct VERTEX {
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
	vector<float> weights;
	vector<unsigned int> boneIndices;
};

struct Bone
{
	string boneName;
	aiBone* bone;
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

std::map<string, UINT> boneMap;
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

	globalInverseTransform = convertXM(pScene->mRootNode->mTransformation.Inverse());
	for (UINT i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* mesh = pScene->mMeshes[i];
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

	/*

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
	*/
	out.close();

    return 0;
}



void processNode(aiNode * node, const aiScene * scene)
{
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


	for (UINT i = 0; i < mesh->mNumBones; i++) {
		UINT BoneIndex = 0;
		string BoneName(mesh->mBones[i]->mName.data);

		if (boneMap.find(BoneName) == boneMap.end()) {
			// Allocate an index for a new bone
			BoneIndex = numBone;
			numBone++;
			boneMap[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = boneMap[BoneName];
		}
		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			UINT VertexID =	mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = mesh->mBones[i]->mWeights[j].mWeight;
			vertices[VertexID].boneIndices.push_back(BoneIndex);
			vertices[VertexID].weights.push_back(Weight);
		}
		//for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
		//	UINT VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
		//	float Weight = mesh->mBones[i]->mWeights[j].mWeight;
		//	vertices[VertexID].boneIndices.push_back(i);
		//	vertices[VertexID].weights.push_back(Weight);
		//}
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
	return;
}