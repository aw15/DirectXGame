#include"stdafx.h"
#include"Parser.h"

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}
void FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray)
{
	const int lPoseCount = pScene->GetPoseCount();

	for (int i = 0; i < lPoseCount; ++i)
	{
		pPoseArray.Add(pScene->GetPose(i));
	}
}
void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal)
{
	if (inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}
void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV)
{
	if (inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}
	FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}
XMFLOAT3 ConvertFloat3(FbxVector4& vec)
{
	XMFLOAT3 ret;
	ret.x = vec.mData[0];
	ret.y = vec.mData[1];
	ret.z = vec.mData[2];

	return ret;
}

XMFLOAT2 ConvertFloat2(FbxVector2& vec)
{
	XMFLOAT2 ret;
	ret.x = vec.mData[0];
	ret.y = vec.mData[1];

	return ret;
}


SkinMesh::SkinMesh(const std::string & fileName)
{
	mFileName = fileName;

	InitializeSdkObjects(mSdkManager, mScene);

	if (mSdkManager)
	{
		// Create the importer.
		int lFileFormat = -1;
		mImporter = FbxImporter::Create(mSdkManager, "");
		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName.c_str(), lFileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}
		// Initialize the importer by providing a filename.
		if (mImporter->Initialize(mFileName.c_str(), lFileFormat) == true)
		{
			// The file is going to be imported at 
			// the end of the first display callback.
			mWindowMessage = "Importing file ";
			mWindowMessage += mFileName;
			mWindowMessage += "\nPlease wait!";
		}
		else
		{
			mWindowMessage = "Unable to open file ";
			mWindowMessage += mFileName;
			mWindowMessage += "\nError reported: ";
			mWindowMessage += mImporter->GetStatus().GetErrorString();
			mWindowMessage += "\nEsc to exit";
		}
	}
	else
	{
		mWindowMessage = "Unable to create the FBX SDK manager";
		mWindowMessage += "\nEsc to exit";
	}

	// initialize cache start and stop time
	mCache_Start = FBXSDK_TIME_INFINITE;
	mCache_Stop = FBXSDK_TIME_MINUS_INFINITE;

	TEST(mWindowMessage);
	LoadFile();
}

SkinMesh::~SkinMesh()
{
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	DestroySdkObjects(mSdkManager, true);
}

bool SkinMesh::LoadFile()
{
	bool result=false;
	
	if (mImporter->Import(mScene) == true)
	{
		FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::DirectX);
		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(mScene);
		}

		// Convert Unit System to what is used in this example, if needed
		FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			//The unit in this example is centimeter.
			FbxSystemUnit::cm.ConvertScene(mScene);
		}
		
		// Get the list of all the animation stack.
		mScene->FillAnimStackNameArray(mAnimStackNameArray);

		// Convert mesh, NURBS and patch into triangle mesh
		FbxGeometryConverter lGeomConverter(mSdkManager);
		lGeomConverter.Triangulate(mScene, /*replace*/true);

		// Get the list of pose in the scene
		FillPoseArray(mScene, mPoseArray);
		result = true;

		// select the base layer from the animation stack
		FbxAnimStack * lCurrentAnimationStack = mScene->FindMember<FbxAnimStack>(mAnimStackNameArray[0]->Buffer());
		if (lCurrentAnimationStack == NULL)
		{
			// this is a problem. The anim stack should be found in the scene!
			return false;
		}

		// we assume that the first animation layer connected to the animation stack is the base layer
		// (this is the assumption made in the FBXSDK)
		mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
		mScene->SetCurrentAnimationStack(lCurrentAnimationStack);

	

		TEST("Success");
	}
	else
	{
		mWindowMessage = "Unable to import file ";
		mWindowMessage += mFileName;
		mWindowMessage += "\nError reported: ";
		mWindowMessage += mImporter->GetStatus().GetErrorString();

		TEST(mWindowMessage);
	}
	// Destroy the importer to release the file.
	mImporter->Destroy();
	mImporter = NULL;

	return result;
}


void SkinMesh::ProcessGeometry(FbxNode* inNode)
{
	if (inNode->GetNodeAttribute())
	{
		switch (inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			//ProcessControlPoints(inNode);
			//if (mHasAnimation)
			//{
			//	ProcessJointsAndAnimations(inNode);
			//}
			ProcessMesh(inNode);
			//AssociateMaterialToMesh(inNode);
			//ProcessMaterials(inNode);
			break;
		}
	}

	for (int i = 0; i < inNode->GetChildCount(); ++i)
	{
		ProcessGeometry(inNode->GetChild(i));
	}
}


void SkinMesh::ProcessMesh(FbxNode* inNode)
{
	FbxMesh* pMesh = inNode->GetMesh();

	if (!pMesh)
		return;

	int lPolygonCount = pMesh->GetPolygonCount();

	FbxVector4 pos, nor;

	unsigned int indx = 0;
	FbxStringList	uvsetName;
	pMesh->GetUVSetNames(uvsetName);
	int numUVSet = uvsetName.GetCount();
	bool unmapped = false;


	for (int i = 0; i<lPolygonCount; i++)
	{
		// ?リゴン内の頂?数(一応、三角?化してるので3?のはずだが?ェック)
		int lPolygonsize = pMesh->GetPolygonSize(i);

		for (int pol = 0; pol<lPolygonsize; pol++)
		{
			VertexData temp;
			int index = pMesh->GetPolygonVertex(i, pol);
			mIndices.push_back(indx);

			pos = pMesh->GetControlPointAt(index);
			pMesh->GetPolygonVertexNormal(i, pol, nor);

			
			temp.pos = ConvertFloat3(pos);
			temp.normal = ConvertFloat3(nor);



			FbxString name = uvsetName.GetStringAt(0);
			FbxVector2 texCoord;
			pMesh->GetPolygonVertexUV(i, pol, name, texCoord, unmapped);
			temp.tex = ConvertFloat2(texCoord);

			mVertexData.push_back(temp);

			++indx;
		}
	}

	//for (unsigned int i = 0; i < mTriangleCount; ++i)
	//{
	//	XMFLOAT3 normal[3];
	//	XMFLOAT3 tangent[3];
	//	XMFLOAT3 binormal[3];
	//	XMFLOAT2 UV[3][2];

	//	for (unsigned int j = 0; j < 3; ++j)
	//	{
	//		int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);

	//		ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);
	//		// We only have diffuse texture
	//		for (int k = 0; k < 1; ++k)
	//		{
	//			ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
	//		}


	//		VertexData temp;
	//		temp.pos = currCtrlPoint->mPosition;
	//		temp.normal = normal[j];
	//		temp.tex = UV[j][0];
	//		// Copy the blending info from each control point
	//		for (unsigned int i = 0; i < currCtrlPoint->mBlendingInfo.size(); ++i)
	//		{
	//			VertexBlendingInfo currBlendingInfo;
	//			currBlendingInfo.mBlendingIndex = currCtrlPoint->mBlendingInfo[i].mBlendingIndex;
	//			currBlendingInfo.mBlendingWeight = currCtrlPoint->mBlendingInfo[i].mBlendingWeight;
	//			temp.mVertexBlendingInfos.push_back(currBlendingInfo);
	//		}
	//		// Sort the blending info so that later we can remove
	//		// duplicated vertices
	//		temp.SortBlendingInfoByWeight();

	//		mVertexData.push_back(temp);
	//		mTriangles.back().mIndices.push_back(vertexCounter);
	//		++vertexCounter;
	//	}
	//}

	//// Now mControlPoints has served its purpose
	//// We can free its memory
	//for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	//{
	//	delete itr->second;
	//}
	//mControlPoints.clear();
}