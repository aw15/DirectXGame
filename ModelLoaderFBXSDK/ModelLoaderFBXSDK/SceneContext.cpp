#include "SceneContext.h"

namespace
{
	const char * SAMPLE_FILENAME = "model/humanoid.fbx";


}


SceneContext::SceneContext(const char * pFileName)
{
	if (pFileName == nullptr)
		pFileName = SAMPLE_FILENAME;

	mFileName = pFileName;

	InitializeSdkObjects(mSdkManager, mScene);

	if (mSdkManager)
	{
		// Create the importer.
		int lFileFormat = -1;
		mImporter = FbxImporter::Create(mSdkManager, "");
		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}

		// Initialize the importer by providing a filename.
		if (mImporter->Initialize(mFileName, lFileFormat) == true)
		{
			printf("Importing file : %s", mFileName);
			// Set scene status flag to ready to load.
			mStatus = MUST_BE_LOADED;
		}
		else
		{

			mWindowMessage = "Unable to open file ";
			mWindowMessage += mFileName;
			mWindowMessage += "\nError reported: ";
			mWindowMessage += mImporter->GetStatus().GetErrorString();
			mWindowMessage += "\nEsc to exit";
			printf(" %s", mWindowMessage.Buffer());
		}
	}
	else
	{
		mWindowMessage = "Unable to create the FBX SDK manager";
		mWindowMessage += "\nEsc to exit";
		printf(" %s", mWindowMessage.Buffer());
	}
}


void SceneContext::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
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

void SceneContext::DestroySdkObjects(bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (mSdkManager) mSdkManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

bool SceneContext::LoadFile()
{
	bool lResult = false;


	if (mImporter->Import(mScene) == true)
	{
		// Set the scene status flag to refresh 
		// the scene in the first timer callback.
		mStatus = MUST_BE_REFRESHED;

		// Convert Axis System to what is used in this example, if needed
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

		// 카메라 필요없음.
		//FillCameraArray(mScene, mCameraArray);

		// Convert mesh, NURBS and patch into triangle mesh
		FbxGeometryConverter lGeomConverter(mSdkManager);
		lGeomConverter.Triangulate(mScene, /*replace*/true);

		// Bake the scene for one frame
		LoadCacheRecu rsive(mScene, mCurrentAnimLayer, mFileName, mSupportVBO);

		// Convert any .PC2 point cache data into the .MC format for 
		// vertex cache deformer playback.
		PreparePointCacheData(mScene, mCache_Start, mCache_Stop);

		// Get the list of pose in the scene
		FillPoseArray(mScene, mPoseArray);

		// Initialize the window message.
		mWindowMessage = "File ";
		mWindowMessage += mFileName;
		mWindowMessage += "\nClick on the right mouse button to enter menu.";
		mWindowMessage += "\nEsc to exit.";

		// Initialize the frame period.
		mFrameTime.SetTime(0, 0, 0, 1, 0, mScene->GetGlobalSettings().GetTimeMode());

		// Print the keyboard shortcuts.
		FBXSDK_printf("Play/Pause Animation: Space Bar.\n");
		FBXSDK_printf("Camera Rotate: Left Mouse Button.\n");
		FBXSDK_printf("Camera Pan: Left Mouse Button + Middle Mouse Button.\n");
		FBXSDK_printf("Camera Zoom: Middle Mouse Button.\n");

		lResult = true;
	}
	else
	{
		// Import failed, set the scene status flag accordingly.
		mStatus = UNLOADED;

		mWindowMessage = "Unable to import file ";
		mWindowMessage += mFileName;
		mWindowMessage += "\nError reported: ";
		mWindowMessage += mImporter->GetStatus().GetErrorString();
	}

	// Destroy the importer to release the file.
	mImporter->Destroy();
	mImporter = NULL;

	return lResult;
}



SceneContext::~SceneContext()
{
}
