
#include"stdafx.h"
#include "MainGame.h"



MainGame::MainGame(HINSTANCE hInstance)
	:Renderer(hInstance)
{

}
MainGame::~MainGame()
{
}

 bool MainGame::Initialize()
{

	 if (!Renderer::Initialize())
		 return false;

	////////////////CAMERA//////////////////
	auto lookVector = mCamera.GetLook3f();
	mCamera.SetPosition(10, 30, 10);
	mCamera.Pitch(0.5*XM_PI);
	///////////////////////텍스쳐 생성//////////////////////
	LoadTexture(L"Textures/bricks.dds", "default");
	LoadTexture(L"Textures/WoodCrate01.dds", "crate");
	LoadTexture(L"Textures/noise01.dds", "fireNoise");
	LoadTexture(L"Textures/alpha01.dds", "fireAlpha");
	LoadTexture(L"Textures/fire01.dds", "fire");
	/////////////////////////////////////////////////////////
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildPSOs();
	///////////////////오브젝트 생성//////////////////////
	LoadAnimationModel("Mesh/GuardB.fbxmesh", PLAYER_MODEL);
//	LoadAnimationModel("ModelLoader/ModelLoader/Model/walking.model", PLAYER_MODEL2);
	BuildShapeGeometry();
	BuildMap();
	BuildFrameResources();
	/////////////////////////////////////////////////
	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}
#pragma endregion
void MainGame::BuildMap()
{
	//for (int i = 0; i < 12; ++i)
	//{
	//	for (int j = 0; j < 12; j++)
	//	{
	//		if (i == 0 || i == 11 || j == 0 || j == 11)
	//		{
	//			CreateObject(SORT::boundary, "default", { (i * 2.0f),0, (j* 2.0f) }, { 2.0f,2.0f,2.0f });
	//		}
	//		else if (((i != 2 && i!=3) || (j != 8 && j!=7)) && ((i != 8 && i != 7) || (j != 2 && j != 3)))
	//		{
	//			CreateObject(SORT::box, "crate" ,{ (i * 2.0f),0, (j* 2.0f) }, { 2.0f,2.0f,2.0f });
	//		}
	//	}
	//}

	CreateObject(SORT::player);
}

void MainGame::OnKeyboardInput(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
	if (GetAsyncKeyState('T') & 0x8000)
	{
		mPlayer[PLAYER2]->Walk(dt);
		mCamera.Walk(10*dt);
	}
	if (GetAsyncKeyState('G') & 0x8000)
	{
		mPlayer[PLAYER2]->Walk(-dt);
		mCamera.Walk(-10 * dt);
	}
	if (GetAsyncKeyState('F') & 0x8000)
	{
		mPlayer[PLAYER2]->Strafe(-dt);
		mCamera.Strafe(-10 * dt);
	}
	if (GetAsyncKeyState('H') & 0x8000)
	{
		mPlayer[PLAYER2]->Strafe(dt);
		mCamera.Strafe(10 * dt);
	}

	if (GetAsyncKeyState('W') & 0x8000)
		mPlayer[PLAYER1]->Walk(dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mPlayer[PLAYER1]->Walk(-dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mPlayer[PLAYER1]->Strafe(-dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mPlayer[PLAYER1]->Strafe(dt);

	if (GetAsyncKeyState('R') & 0x8000)
	{
		CreateObject(SORT::bomb,"default", { 0,0,0 }, {0,0,0}, PLAYER1);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
	}

	mCamera.UpdateViewMatrix();
	mPlayer[PLAYER1]->NumFramesDirty = gNumFrameResources;
	mPlayer[PLAYER2]->NumFramesDirty = gNumFrameResources;
}

void MainGame::Update(const GameTimer & gt)
{
	OnKeyboardInput(gt);
	for (auto& player : mObjectLayer[SORT::player])
	{
		player->Update(gt.DeltaTime());
		for (auto& box : mObjectLayer[SORT::box])
		{
			box->Update(gt.DeltaTime());
			if (player->NumFramesDirty > 0)
			{
				if (player->mBoundingBox.Intersects(box->mBoundingBox))
				{
					player->Move(INVERSE);
				}
			}
		}

		for (auto& box : mObjectLayer[SORT::boundary])
		{
			box->Update(gt.DeltaTime());
			if (player->NumFramesDirty > 0)
			{
				if (player->mBoundingBox.Intersects(box->mBoundingBox))
				{
					player->Move(INVERSE);
				}
			}
		}

		player->mDir = { 0,0,0 };
	}
	
	for (int i =0; i<mObjectLayer[SORT::bomb].size();i++)
	{
		mObjectLayer[SORT::bomb][i]->Update(gt.DeltaTime());
		mObjectLayer[SORT::bomb][i]->NumFramesDirty = gNumFrameResources;
		if (((Bomb*)mObjectLayer[SORT::bomb][i])->GetLifetime() > EXPLOSIVE_COOLTIME)
		{
			CreateObject(SORT::fire,"default", mObjectLayer[SORT::bomb][i]->GetPosition3f());
			DestroyItem(i, mObjectLayer[SORT::bomb]);
		}
	}

	for (int i = 0; i < mObjectLayer[SORT::fire].size(); i++)
	{
		mObjectLayer[SORT::fire][i]->Update(gt.DeltaTime());
		for (int j=0 ; j < mObjectLayer[SORT::box].size();j++)
		{
			if (mObjectLayer[SORT::fire][i]->mBoundingBox.Contains(mObjectLayer[SORT::box][j]->mBoundingBox))
			{
				TEST("collision");
				DestroyItem(j, mObjectLayer[SORT::box]);
			}

		}
		if (((Fire*)mObjectLayer[SORT::fire][i])->GetLifetime() > EXPLOSIVE_COOLTIME)
		{
			DestroyItem(i, mObjectLayer[SORT::fire]);
		}
	}

	Renderer::Update(gt);
}

void MainGame::Draw(const GameTimer & gt)
{
	Renderer::Draw(gt);
}



void MainGame::CreateObject(const SORT category, const char* materialName , const XMFLOAT3 & position, const XMFLOAT3 & scale,const UINT playerID)
{
	if (category == SORT::player)
	{
		auto player1 = std::make_unique<Player>();
		XMStoreFloat4x4(&player1->World, XMMatrixScaling(0.01f, 0.01f, 0.01f)*XMMatrixRotationX(0.5*XM_PI)*XMMatrixTranslation(5.0f, 0.0f, 15.5f));
		XMStoreFloat4x4(&player1->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		player1->ObjCBIndex = mObjectConstantCount++;
		player1->Mat = mMaterials[materialName].get();
		player1->Geo = mGeometries[PLAYER_MODEL].get();
		player1->mTag = PLAYER;
		player1->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		player1->IndexCount = player1->Geo->DrawArgs[PLAYER_MODEL].IndexCount;
		player1->StartIndexLocation = player1->Geo->DrawArgs[PLAYER_MODEL].StartIndexLocation;
		player1->BaseVertexLocation = player1->Geo->DrawArgs[PLAYER_MODEL].BaseVertexLocation;
		player1->mStandardBox = &(player1->Geo->DrawArgs[PLAYER_MODEL].Bounds);
		mPlayer[PLAYER1] = player1.get();
		mAllRitems.push_back(std::move(player1));
		mObjectLayer[category].push_back(mAllRitems.back().get());

		auto player2 = std::make_unique<Player>();
		XMStoreFloat4x4(&player2->World, XMMatrixScaling(0.01f, 0.01f, 0.01f)*XMMatrixRotationX(0.5*XM_PI)*XMMatrixTranslation(15.5f, 0.0f, 5.0f));
		XMStoreFloat4x4(&player2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		player2->ObjCBIndex = mObjectConstantCount++;
		player2->Mat = mMaterials[materialName].get();
		player2->Geo = mGeometries[PLAYER_MODEL].get();
		player2->mTag = PLAYER;
		player2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		player2->IndexCount = player2->Geo->DrawArgs[PLAYER_MODEL].IndexCount;
		player2->StartIndexLocation = player2->Geo->DrawArgs[PLAYER_MODEL].StartIndexLocation;
		player2->BaseVertexLocation = player2->Geo->DrawArgs[PLAYER_MODEL].BaseVertexLocation;
		player2->mStandardBox = &(player2->Geo->DrawArgs[PLAYER_MODEL].Bounds);
		mPlayer[PLAYER2] = player2.get();
		mAllRitems.push_back(std::move(player2));
		mObjectLayer[category].push_back(mAllRitems.back().get());

	}
	else if (category == SORT::bomb)
	{
		Player* player;
		player = mPlayer[playerID];

		auto bomb = std::make_unique<Bomb>();
		XMStoreFloat4x4(&bomb->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(player->World._41, player->World._42, player->World._43));
		XMStoreFloat4x4(&bomb->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		bomb->mPosition = { player->World._41, player->World._42, player->World._43 };
		bomb->SetPlayerID(playerID);
		bomb->mTag = BOMB;
		bomb->ObjCBIndex = mObjectConstantCount++;
		bomb->Mat = mMaterials[materialName].get();
		bomb->Geo = mGeometries["shapeGeo"].get();
		bomb->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		bomb->IndexCount = bomb->Geo->DrawArgs["box"].IndexCount;
		bomb->StartIndexLocation = bomb->Geo->DrawArgs["box"].StartIndexLocation;
		bomb->BaseVertexLocation = bomb->Geo->DrawArgs["box"].BaseVertexLocation;
		bomb->mStandardBox = &(bomb->Geo->DrawArgs["box"].Bounds);
		mAllRitems.push_back(std::move(bomb));
		mObjectLayer[category].push_back(mAllRitems.back().get());
		RebuildFrameResouce();
	}
	else if (category == SORT::fire)
	{
		auto fire = std::make_unique<Fire>();
		XMStoreFloat4x4(&fire->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(position.x , position.y, position.z));
		XMStoreFloat4x4(&fire->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		fire->mScale = { 1,1,1 };
		fire->mPosition = { position.x,position.y,position.z };
		fire->mTag = BOMB;
		fire->ObjCBIndex = mObjectConstantCount++;
		fire->Mat = mMaterials[materialName].get();
		fire->Geo = mGeometries["shapeGeo"].get();
		fire->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		fire->IndexCount = fire->Geo->DrawArgs["box"].IndexCount;
		fire->StartIndexLocation = fire->Geo->DrawArgs["box"].StartIndexLocation;
		fire->BaseVertexLocation = fire->Geo->DrawArgs["box"].BaseVertexLocation;
		fire->mStandardBox = &(fire->Geo->DrawArgs["box"].Bounds);
		mAllRitems.push_back(std::move(fire));
		mObjectLayer[category].push_back(mAllRitems.back().get());

		for (int i = -1; i <= 1; i+=2)
		{
			auto fire = std::make_unique<Fire>();
			XMStoreFloat4x4(&fire->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(position.x+2*i, position.y, position.z));
			XMStoreFloat4x4(&fire->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
			fire->mScale = { 1,1,1 };
			fire->mPosition = { position.x,position.y,position.z };
			fire->mTag = BOMB;
			fire->ObjCBIndex = mObjectConstantCount++;
			fire->Mat = mMaterials[materialName].get();
			fire->Geo = mGeometries["shapeGeo"].get();
			fire->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			fire->IndexCount = fire->Geo->DrawArgs["box"].IndexCount;
			fire->StartIndexLocation = fire->Geo->DrawArgs["box"].StartIndexLocation;
			fire->BaseVertexLocation = fire->Geo->DrawArgs["box"].BaseVertexLocation;
			fire->mStandardBox = &(fire->Geo->DrawArgs["box"].Bounds);
			mAllRitems.push_back(std::move(fire));
			mObjectLayer[category].push_back(mAllRitems.back().get());

			fire = std::make_unique<Fire>();
			XMStoreFloat4x4(&fire->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(position.x , position.y, position.z + 2*i));
			XMStoreFloat4x4(&fire->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
			fire->mScale = { 1,1,1 };
			fire->mPosition = { position.x,position.y,position.z };
			fire->mTag = BOMB;
			fire->ObjCBIndex = mObjectConstantCount++;
			fire->Mat = mMaterials[materialName].get();
			fire->Geo = mGeometries["shapeGeo"].get();
			fire->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			fire->IndexCount = fire->Geo->DrawArgs["box"].IndexCount;
			fire->StartIndexLocation = fire->Geo->DrawArgs["box"].StartIndexLocation;
			fire->BaseVertexLocation = fire->Geo->DrawArgs["box"].BaseVertexLocation;
			fire->mStandardBox = &(fire->Geo->DrawArgs["box"].Bounds);
			mAllRitems.push_back(std::move(fire));
			mObjectLayer[category].push_back(mAllRitems.back().get());
		}
		RebuildFrameResouce();
	}
	else if (category == SORT::box)
	{
		auto boxRitem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(scale.x,scale.y,scale.z)*XMMatrixTranslation(position.x, position.y, position.z));
		XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1, 1, 1));
		boxRitem->ObjCBIndex = mObjectConstantCount++;
		boxRitem->Mat = mMaterials[materialName].get();
		boxRitem->Geo = mGeometries["shapeGeo"].get();
		boxRitem->mTag = BOX;
		boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
		boxRitem->mStandardBox = &(boxRitem->Geo->DrawArgs["box"].Bounds);
		mAllRitems.push_back(std::move(boxRitem));
		mObjectLayer[category].push_back(mAllRitems.back().get());
	}
	else if (category == SORT::boundary)
	{
		auto boxRitem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(scale.x, scale.y, scale.z)*XMMatrixTranslation(position.x, position.y, position.z));
		XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1, 1, 1));
		boxRitem->ObjCBIndex = mObjectConstantCount++;
		boxRitem->Mat = mMaterials[materialName].get();
		boxRitem->Geo = mGeometries["shapeGeo"].get();
		boxRitem->mTag = BOX;
		boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
		boxRitem->mStandardBox = &(boxRitem->Geo->DrawArgs["box"].Bounds);
		mAllRitems.push_back(std::move(boxRitem));
		mObjectLayer[category].push_back(mAllRitems.back().get());
	}
}


void MainGame::DestroyItem(int index, std::vector<RenderItem*>& items )
{
	if (!items.empty())
	{
		auto item = items[index];
		auto itemAtAllitems = find_if(mAllRitems.begin(), mAllRitems.end(), [&item](std::unique_ptr<RenderItem>& target)
		{
			return target.get() == item;
		});
		items.erase(items.begin() + index);
		itemAtAllitems->release();
		mAllRitems.erase(itemAtAllitems);
		RebuildFrameResouce();
	}
}

void MainGame::RebuildFrameResouce()
{
	for (int i = 0; i < mFrameResources.size(); i++)
	{
  		mFrameResources[i]->RebuildResource(md3dDevice.Get(), mAllRitems.size());
	}
	mObjectConstantCount = 0;
	for (auto& item : mAllRitems)
	{
		item->ObjCBIndex = mObjectConstantCount++;
		item->NumFramesDirty = gNumFrameResources;
	}
}
  