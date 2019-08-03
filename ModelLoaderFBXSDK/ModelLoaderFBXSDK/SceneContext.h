#pragma once
#include<fbxsdk.h>
#include<iostream>
#include<string>




class SceneContext
{
private:
	enum Status
	{
		UNLOADED,               // Unload file or load failure;
		MUST_BE_LOADED,         // Ready for loading file;
		MUST_BE_REFRESHED,      // Something changed and redraw needed;
		REFRESHED               // No redraw needed.
	};

public:
	SceneContext(const char * pFileName);
	~SceneContext();
	//Init & Destroy
	void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	void DestroySdkObjects(bool pExitStatus);

	bool LoadFile();


	//Get
	Status GetStatus() const { return mStatus; }

	const char * mFileName;

	//FBX Manager
	FbxManager * mSdkManager;
	FbxScene * mScene;
	FbxImporter * mImporter;


	FbxArray<FbxString*> mAnimStackNameArray;
private:
	mutable Status mStatus;
	mutable FbxString mWindowMessage;
};

