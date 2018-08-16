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

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}
// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its
// global position at the current time.
FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = NULL, FbxAMatrix* pParentGlobalPosition = NULL)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}
FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
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

		FbxTakeInfo* lCurrentTakeInfo = mScene->GetTakeInfo(*(mAnimStackNameArray[0]));
		if (lCurrentTakeInfo)
		{
			mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			// Take the time line value
			FbxTimeSpan lTimeLineTimeSpan;
			mScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

			mStart = lTimeLineTimeSpan.GetStart();
			mStop = lTimeLineTimeSpan.GetStop();
		}

		// check for smallest start with cache start
		if (mCache_Start < mStart)
			mStart = mCache_Start;

		// check for biggest stop with cache stop
		if (mCache_Stop  > mStop)
			mStop = mCache_Stop;

		// Initialize the frame period.
		mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());

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

//void SkinMesh::OnDisplay()
//{
//	if (mStop > mStart)
//	{
//		// Set the scene status flag to refresh 
//		// the scene in the next timer callback
//		mCurrentTime += mFrameTime;
//
//		if (mCurrentTime > mStop)
//		{
//			mCurrentTime = mStart;
//		}
//	}
//
//	FbxPose * lPose = NULL;
//	lPose = mScene->GetPose(1);
//	// If one node is selected, draw it and its children.
//	FbxAMatrix lDummyGlobalPosition;
//
//	DrawNodeRecursive(mScene->GetRootNode(), mCurrentTime, mCurrentAnimLayer, lDummyGlobalPosition, lPose);
//}
//
//void SkinMesh::DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
//	FbxAMatrix& pParentGlobalPosition, FbxPose* pPose)
//{
//	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
//	if (pNode->GetNodeAttribute())
//	{
//		// Geometry offset.
//		// it is not inherited by the children.
//		FbxAMatrix lGeometryOffset = GetGeometry(pNode);
//		FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
//
//		DrawNode(pNode, pTime, pAnimLayer, pParentGlobalPosition, lGlobalOffPosition, pPose);
//	}
//
//	const int lChildCount = pNode->GetChildCount();
//	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
//	{
//		DrawNodeRecursive(pNode->GetChild(lChildIndex), pTime, pAnimLayer, lGlobalPosition, pPose);
//	}
//}
//
//void SkinMesh::DrawNode(FbxNode * pNode, FbxTime & pTime, FbxAnimLayer * pAnimLayer, FbxAMatrix & pParentGlobalPosition, FbxAMatrix & pGlobalPosition, FbxPose * pPose)
//{
//	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
//
//	if (lNodeAttribute)
//	{
//		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
//		{
//			DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose);
//		}
//	}
//}
//
//// Draw the vertices of a mesh.
//void SkinMesh::DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
//	FbxAMatrix& pGlobalPosition, FbxPose* pPose)
//{
//	FbxMesh* lMesh = pNode->GetMesh();
//	const int lVertexCount = lMesh->GetControlPointsCount();
//
//	// No vertex to draw.
//	if (lVertexCount == 0)
//	{
//		return;
//	}
//
//
//	// If it has some defomer connection, update the vertices position
//	const bool lHasVertexCache = lMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
//		(static_cast<FbxVertexCacheDeformer*>(lMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active.Get();
//	const bool lHasShape = lMesh->GetShapeCount() > 0;
//	const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
//	const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;
//
//	FbxVector4* lVertexArray = NULL;
//	if (!lMeshCache || lHasDeformation)
//	{
//		lVertexArray = new FbxVector4[lVertexCount];
//		memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
//	}
//
//
//
//	if (lHasDeformation)
//	{
//		// Active vertex cache deformer will overwrite any other deformer
//		if (lHasVertexCache)
//		{
//			ReadVertexCacheData(lMesh, pTime, lVertexArray);
//		}
//		else
//		{
//			if (lHasShape)
//			{
//				// Deform the vertex array with the shapes.
//				ComputeShapeDeformation(lMesh, pTime, pAnimLayer, lVertexArray);
//			}
//			//we need to get the number of clusters
//			const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
//			int lClusterCount = 0;
//			for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
//			{
//				lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
//			}
//			if (lClusterCount)
//			{
//				// Deform the vertex array with the skin deformer.
//				ComputeSkinDeformation(pGlobalPosition, lMesh, pTime, lVertexArray, pPose);
//			}
//		}
//
//		if (lMeshCache)
//			lMeshCache->UpdateVertexPosition(lMesh, lVertexArray);
//	}
//
//	glPushMatrix();
//	glMultMatrixd((const double*)pGlobalPosition);
//
//	if (lMeshCache)
//	{
//		lMeshCache->BeginDraw(pShadingMode);
//		const int lSubMeshCount = lMeshCache->GetSubMeshCount();
//		for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex)
//		{
//			if (pShadingMode == SHADING_MODE_SHADED)
//			{
//				const FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lIndex);
//				if (lMaterial)
//				{
//					const MaterialCache * lMaterialCache = static_cast<const MaterialCache *>(lMaterial->GetUserDataPtr());
//					if (lMaterialCache)
//					{
//						lMaterialCache->SetCurrentMaterial();
//					}
//				}
//				else
//				{
//					// Draw green for faces without material
//					MaterialCache::SetDefaultMaterial();
//				}
//			}
//
//			lMeshCache->Draw(lIndex, pShadingMode);
//		}
//		lMeshCache->EndDraw();
//	}
//	else
//	{
//		// OpenGL driver is too lower and use Immediate Mode
//		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
//		const int lPolygonCount = lMesh->GetPolygonCount();
//		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
//		{
//			const int lVerticeCount = lMesh->GetPolygonSize(lPolygonIndex);
//			glBegin(GL_LINE_LOOP);
//			for (int lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
//			{
//				glVertex3dv((GLdouble *)lVertexArray[lMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex)]);
//			}
//			glEnd();
//		}
//	}
//
//	glPopMatrix();
//
//	delete[] lVertexArray;
//}