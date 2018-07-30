
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
	////////////////////////////////////////////////////
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildPSOs();
	///////////////////오브젝트 생성//////////////////////
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

void MainGame::BuildMap()
{
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; j++)
		{
			if (((i != 2 && i!=3) || (j != 8 && j!=7)) && ((i != 8 && i != 7) || (j != 2 && j != 3)))
			{
				auto boxRitem = std::make_unique<RenderItem>();
				XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2,2,2)*XMMatrixTranslation(i*2.0f, 0.0f, j*2.0f));
				XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1, 1, 1));
				boxRitem->ObjCBIndex = m_objectConstantCount++;
				boxRitem->Mat = mMaterials["crate"].get();
				boxRitem->Geo = mGeometries["shapeGeo"].get();
				boxRitem->mTag = BOX;
				boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
				boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
				boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
				boxRitem->mStandardBox = &(boxRitem->Geo->DrawArgs["box"].Bounds);
				mAllRitems.push_back(std::move(boxRitem));
			}
		}
	}
	 //All the render items are opaque.
	for (auto& e : mAllRitems)
		mBoxRitems.push_back(e.get());

	auto player2 = std::make_unique<Player>();
	XMStoreFloat4x4(&player2->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(15.5f, 0.0f, 5.0f));
	XMStoreFloat4x4(&player2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	player2->ObjCBIndex = m_objectConstantCount++;
	player2->Mat = mMaterials["default"].get();
	player2->Geo = mGeometries["shapeGeo"].get();
	player2->mTag = PLAYER;
	player2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	player2->IndexCount = player2->Geo->DrawArgs["cylinder"].IndexCount;
	player2->StartIndexLocation = player2->Geo->DrawArgs["cylinder"].StartIndexLocation;
	player2->BaseVertexLocation = player2->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	player2->mStandardBox = &(player2->Geo->DrawArgs["cylinder"].Bounds);
	m_player2 = player2.get();
	mAllRitems.push_back(std::move(player2));
	mPlayerRitems.push_back(mAllRitems.back().get());

	auto player1 = std::make_unique<Player>();
	XMStoreFloat4x4(&player1->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(5.0f, 0.0f, 15.5f));
	XMStoreFloat4x4(&player1->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	player1->ObjCBIndex = m_objectConstantCount++;
	player1->Mat = mMaterials["default"].get();
	player1->Geo = mGeometries["shapeGeo"].get();
	player1->mTag = PLAYER;
	player1->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	player1->IndexCount = player1->Geo->DrawArgs["cylinder"].IndexCount;
	player1->StartIndexLocation = player1->Geo->DrawArgs["cylinder"].StartIndexLocation;
	player1->BaseVertexLocation = player1->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	player1->mStandardBox = &(player1->Geo->DrawArgs["cylinder"].Bounds);
	m_player1 = player1.get();
	mAllRitems.push_back(std::move(player1));
	mPlayerRitems.push_back(mAllRitems.back().get());
}

void MainGame::OnKeyboardInput(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
	if (GetAsyncKeyState('T') & 0x8000)
	{
	//	m_player2->Walk(dt);
		mCamera.Walk(10*dt);
	}
	if (GetAsyncKeyState('G') & 0x8000)
	{
	//	m_player2->Walk(-dt);
		mCamera.Walk(-10 * dt);
	}
	if (GetAsyncKeyState('F') & 0x8000)
	{
		//m_player2->Strafe(-dt);
		mCamera.Strafe(-10 * dt);
	}
	if (GetAsyncKeyState('H') & 0x8000)
	{
	//	m_player2->Strafe(dt);
		mCamera.Strafe(10 * dt);
	}

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
	}

	mCamera.UpdateViewMatrix();
	m_player1->NumFramesDirty = gNumFrameResources;
	m_player2->NumFramesDirty = gNumFrameResources;
}

void MainGame::Update(const GameTimer & gt)
{
	OnKeyboardInput(gt);
	for (auto& player : mPlayerRitems)
	{
		player->Update(gt.DeltaTime());
		for (auto& box : mBoxRitems)
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
	
	for (int i =0; i<mBombRitems.size();i++)
	{
		mBombRitems[i]->Update(gt.DeltaTime());
		mBombRitems[i]->NumFramesDirty = gNumFrameResources;
		if (((Bomb*)mBombRitems[i])->GetLifetime() > EXPLOSIVE_COOLTIME)
		{
			CreateFire(mBombRitems[i]->GetPosition3f());
			DestroyItem(i,mBombRitems);
		}
	}

	for (int i = 0; i < mFireRitems.size(); i++)
	{
		//auto cameraPosition = mCamera.GetPosition3f();
		//auto position = mFireRitems[i]->GetPosition3f();
		//auto angle = atan2f(position.y - cameraPosition.y, position.z - cameraPosition.z)*(180.0f/XM_PI);
		//auto rotation = (float)angle * 0.0174532925f;
		//mFireRitems[i]->mRotation.x = 0.5*XM_PI;
		//mFireRitems[i]->NumFramesDirty = gNumFrameResources;
		mFireRitems[i]->Update(gt.DeltaTime());
		if (((Fire*)mFireRitems[i])->GetLifetime() > EXPLOSIVE_COOLTIME)
		{
			DestroyItem(i, mFireRitems);
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

	auto bomb = std::make_unique<Bomb>();
	XMStoreFloat4x4(&bomb->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(player->World._41, player->World._42, player->World._43));
	XMStoreFloat4x4(&bomb->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	bomb->mPosition = { player->World._41, player->World._42, player->World._43 };
	bomb->SetPlayerID(playerID);
	bomb->mTag = BOMB;
	bomb->ObjCBIndex = m_objectConstantCount++;
	bomb->Mat = mMaterials["default"].get();
	bomb->Geo = mGeometries["shapeGeo"].get();
	bomb->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	bomb->IndexCount = bomb->Geo->DrawArgs["box"].IndexCount;
	bomb->StartIndexLocation = bomb->Geo->DrawArgs["box"].StartIndexLocation;
	bomb->BaseVertexLocation = bomb->Geo->DrawArgs["box"].BaseVertexLocation;
	bomb->mStandardBox = &(bomb->Geo->DrawArgs["box"].Bounds);
	mAllRitems.push_back(std::move(bomb));
	mBombRitems.push_back(mAllRitems.back().get());
	RebuildFrameResouce();
}

void MainGame::CreateFire(XMFLOAT3& position)
{
	auto fire = std::make_unique<Fire>();
	XMStoreFloat4x4(&fire->World, XMMatrixScaling(1.0f, 1.0f, 1.0f)*XMMatrixTranslation(position.x,position.y,position.z));
	XMStoreFloat4x4(&fire->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	fire->mScale = { 1,1,1 };
	fire->mPosition = { position.x,position.y,position.z };
	fire->mTag = BOMB;
	fire->ObjCBIndex = m_objectConstantCount++;
	fire->Mat = mMaterials["default"].get();
	fire->Geo = mGeometries["shapeGeo"].get();
	fire->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	fire->IndexCount = fire->Geo->DrawArgs["box"].IndexCount;
	fire->StartIndexLocation = fire->Geo->DrawArgs["box"].StartIndexLocation;
	fire->BaseVertexLocation = fire->Geo->DrawArgs["box"].BaseVertexLocation;
	fire->mStandardBox = &(fire->Geo->DrawArgs["box"].Bounds);
	mAllRitems.push_back(std::move(fire));
	mFireRitems.push_back(mAllRitems.back().get());
	RebuildFrameResouce();
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
  