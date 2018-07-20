
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

	 if (FAILED(Renderer::Initialize()))
	 {
		 return false;
	 }
	/////////////////////////////////////////////////
	auto lookVector = mCamera.GetLook3f();
	mCamera.SetPosition(10, 30, 10);
	mCamera.Pitch(0.5*XM_PI);
	///////////////////////////////////////////////////
	//LoadTexture(L"Textures/WoodCrate01.dds", "crate");
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


 /*geo->DrawArgs["box"] = boxSubmesh;
 geo->DrawArgs["grid"] = gridSubmesh;
 geo->DrawArgs["sphere"] = sphereSubmesh;
 geo->DrawArgs["cylinder"] = cylinderSubmesh;*/


 //mMaterials["bricks0"] = std::move(bricks0);
 //mMaterials["stone0"] = std::move(stone0);
 //mMaterials["tile0"] = std::move(tile0);
 //mMaterials["crate0"] = std::move(crate0);
 //mMaterials["crate1"] = std::move(crate1);
 //mMaterials["crate2"] = std::move(crate2);

void MainGame::BuildMap()
{
	//for (int i = 0; i < 10; ++i)
	//{
	//	for (int j = 0; j < 10; j++)
	//	{
	//		if (((i != 2 && i!=3) || (j != 8 && j!=7)) && ((i != 8 && i != 7) || (j != 2 && j != 3)))
	//		{
	//			auto boxRitem = std::make_unique<RenderItem>();
	//			XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(i*2.0f, 0.0f, j*2.0f));
	//			XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	//			boxRitem->ObjCBIndex = m_objectConstantCount++;
	//			boxRitem->Mat = mMaterials["crate0"].get();
	//			boxRitem->Geo = mGeometries["shapeGeo"].get();
	//			boxRitem->Tag = BOX;
	//			boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//			boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	//			boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	//			boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	//			boxRitem->m_standardBox = &(boxRitem->Geo->DrawArgs["box"].Bounds);
	//			mAllRitems.push_back(std::move(boxRitem));
	//		}
	//	}
	//}
	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.get());

	auto player2 = std::make_unique<Player>();
	XMStoreFloat4x4(&player2->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(15.5f, 0.0f, 5.0f));
	XMStoreFloat4x4(&player2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	player2->ObjCBIndex = m_objectConstantCount++;
	player2->Mat = mMaterials["crate0"].get();
	player2->Geo = mGeometries["shapeGeo"].get();
	player2->Tag = PLAYER;
	player2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	player2->IndexCount = player2->Geo->DrawArgs["cylinder"].IndexCount;
	player2->StartIndexLocation = player2->Geo->DrawArgs["cylinder"].StartIndexLocation;
	player2->BaseVertexLocation = player2->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	player2->m_standardBox = &(player2->Geo->DrawArgs["cylinder"].Bounds);
	m_player2 = player2.get();
	mAllRitems.push_back(std::move(player2));
	mPlayerRitems.push_back(mAllRitems.back().get());

	auto player1 = std::make_unique<Player>();
	XMStoreFloat4x4(&player1->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(5.0f, 0.0f, 15.5f));
	XMStoreFloat4x4(&player1->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	player1->ObjCBIndex = m_objectConstantCount++;
	player1->Mat = mMaterials["crate0"].get();
	player1->Geo = mGeometries["shapeGeo"].get();
	player1->Tag = PLAYER;
	player1->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	player1->IndexCount = player1->Geo->DrawArgs["cylinder"].IndexCount;
	player1->StartIndexLocation = player1->Geo->DrawArgs["cylinder"].StartIndexLocation;
	player1->BaseVertexLocation = player1->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	player1->m_standardBox = &(player1->Geo->DrawArgs["cylinder"].Bounds);
	m_player1 = player1.get();
	mAllRitems.push_back(std::move(player1));
	mPlayerRitems.push_back(mAllRitems.back().get());
}

void MainGame::OnKeyboardInput(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
		if (GetAsyncKeyState('T') & 0x8000)
			m_player2->Walk(dt);

		if (GetAsyncKeyState('G') & 0x8000)
			m_player2->Walk(-dt);

		if (GetAsyncKeyState('F') & 0x8000)
			m_player2->Strafe(-dt);

		if (GetAsyncKeyState('H') & 0x8000)
			m_player2->Strafe(dt);

		if (GetAsyncKeyState('W') & 0x8000)
			m_player1->Walk(dt);

		if (GetAsyncKeyState('S') & 0x8000)
			m_player1->Walk(-dt);

		if (GetAsyncKeyState('A') & 0x8000)
			m_player1->Strafe(-dt);

		if (GetAsyncKeyState('D') & 0x8000)
			m_player1->Strafe(dt);

		if (GetAsyncKeyState('R') & 0x8000)
		{
			CreateBomb(PLAYER1);
		}

		if (GetAsyncKeyState('Q') & 0x8000)
		{
		
			DestroyBomb();
		}

	mCamera.UpdateViewMatrix();
	m_player1->NumFramesDirty = gNumFrameResources;
	m_player2->NumFramesDirty = gNumFrameResources;

}

void MainGame::Update(const GameTimer & gt)
{
	OnKeyboardInput(gt);
	for (auto& e : mAllRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0)
		{
			e->Update(gt.DeltaTime());
			if (e->Tag == PLAYER)
			{
				
				for (auto& item : mAllRitems)
				{
					if (item->Tag == BOX && item->m_boundingBox.Intersects(e->m_boundingBox))
					{
						e->Move(INVERSE);
						break;
					}
				}
				e->m_dir = { 0,0,0 };
			}

		}
	}

	Renderer::Update(gt);
}

void MainGame::Draw(const GameTimer & gt)
{
	Renderer::Draw(gt);
}

void MainGame::CreateBomb(int playerID)
{
	Player* player;
	if (playerID == PLAYER1)
		player = m_player1;
	if (playerID == PLAYER2)
		player = m_player2;

	auto bomb = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&bomb->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(player->World._41, player->World._42, player->World._43));
	XMStoreFloat4x4(&bomb->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	bomb->ObjCBIndex = m_objectConstantCount++;
	bomb->Mat = mMaterials["bricks0"].get();
	bomb->Geo = mGeometries["shapeGeo"].get();
	bomb->Tag = BOMB;
	bomb->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	bomb->IndexCount = bomb->Geo->DrawArgs["sphere"].IndexCount;
	bomb->StartIndexLocation = bomb->Geo->DrawArgs["sphere"].StartIndexLocation;
	bomb->BaseVertexLocation = bomb->Geo->DrawArgs["sphere"].BaseVertexLocation;
	bomb->m_standardBox = &(bomb->Geo->DrawArgs["sphere"].Bounds);
	mAllRitems.push_back(std::move(bomb));
	mBombRitems.push_back(mAllRitems.back().get());
	RebuildFrameResouce();
}


void MainGame::DestroyBomb()
{
	if (!mBombRitems.empty())
	{
		auto bomb = mBombRitems.back();
		auto bombAtAllitems = find_if(mAllRitems.begin(), mAllRitems.end(), [&bomb](std::unique_ptr<RenderItem>& item)
		{
			return item.get() == bomb;
		});
		mBombRitems.erase(mBombRitems.end() - 1);
		bombAtAllitems->release();
		mAllRitems.erase(bombAtAllitems);
		TEST("DELETE");
		RebuildFrameResouce();
	}
}

void MainGame::RebuildFrameResouce()
{
	for (int i = 0; i < mFrameResources.size(); i++)
	{
  		mFrameResources[i]->RebuildResource(md3dDevice.Get(), mAllRitems.size());
	}
	m_objectConstantCount = 0;
	for (auto& item : mAllRitems)
	{
		item->ObjCBIndex = m_objectConstantCount++;
		item->NumFramesDirty = gNumFrameResources;
	}
}
  