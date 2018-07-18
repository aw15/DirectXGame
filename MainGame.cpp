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

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; j++)
		{
			if (((i != 2 && i!=3) || (j != 8 && j!=7)) && ((i != 8 && i != 7) || (j != 2 && j != 3)))
			{
				auto boxRitem = std::make_unique<RenderItem>();
				XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(i*2.0f, 0.0f, j*2.0f));
				XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
				boxRitem->ObjCBIndex = objIndex++;
				boxRitem->Mat = mMaterials["crate0"].get();
				boxRitem->Geo = mGeometries["shapeGeo"].get();
				boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
				boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
				boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
				mAllRitems.push_back(std::move(boxRitem));
			}
		}
	}
	auto player1 = std::make_unique<Player>();
	XMStoreFloat4x4(&player1->World, XMMatrixScaling(2.0f, 1.0f, 2.0f)*XMMatrixTranslation(4.0f, 0.0f, 16.0f));
	XMStoreFloat4x4(&player1->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	player1->ObjCBIndex = objIndex++;
	player1->Mat = mMaterials["bricks0"].get();
	player1->Geo = mGeometries["shapeGeo"].get();
	player1->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	player1->IndexCount = player1->Geo->DrawArgs["cylinder"].IndexCount;
	player1->StartIndexLocation = player1->Geo->DrawArgs["cylinder"].StartIndexLocation;
	player1->BaseVertexLocation = player1->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	m_player1 = player1.get();
	mAllRitems.push_back(std::move(player1));


	auto player2 = std::make_unique<Player>();
	XMStoreFloat4x4(&player2->World, XMMatrixScaling(2.0f, 1.0f, 2.0f)*XMMatrixTranslation(16.0f, 0.0f, 4.0f));
	XMStoreFloat4x4(&player2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	player2->ObjCBIndex = objIndex++;
	player2->Mat = mMaterials["bricks0"].get();
	player2->Geo = mGeometries["shapeGeo"].get();
	player2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	player2->IndexCount = player2->Geo->DrawArgs["cylinder"].IndexCount;
	player2->StartIndexLocation = player2->Geo->DrawArgs["cylinder"].StartIndexLocation;
	player2->BaseVertexLocation = player2->Geo->DrawArgs["cylinder"].BaseVertexLocation;
	m_player2 = player2.get();
	mAllRitems.push_back(std::move(player2));

	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.get());
}

void MainGame::OnKeyboardInput(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
	if (GetAsyncKeyState('W') & 0x8000)
		mCamera.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera.Strafe(10.0f*dt);

	if (GetAsyncKeyState('T') & 0x8000)
		m_player1->Walk(m_player1->m_speed*dt);

	if (GetAsyncKeyState('G') & 0x8000)
		m_player1->Walk(-m_player1->m_speed*dt);

	if (GetAsyncKeyState('F') & 0x8000)
		m_player1->Strafe(-m_player1->m_speed*dt);

	if (GetAsyncKeyState('H') & 0x8000)
		m_player1->Strafe(m_player1->m_speed*dt);

	if (GetAsyncKeyState('R') & 0x8000)
	{
		isCreate = true;

	}

	mCamera.UpdateViewMatrix();
	m_player1->NumFramesDirty = gNumFrameResources;
}

void MainGame::Update(const GameTimer & gt)
{
	OnKeyboardInput(gt);
	if (isCreate)
	{
		auto boxRitem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(22.0f, 0.0f, 22.0f));
		XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		boxRitem->ObjCBIndex = objIndex++;
		boxRitem->Mat = mMaterials["crate0"].get();
		boxRitem->Geo = mGeometries["shapeGeo"].get();
		boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
		mAllRitems.push_back(std::move(boxRitem));
		mOpaqueRitems.push_back(mAllRitems.back().get());
		RebuildFrameResouce();
		isCreate = false;
	}
	Renderer::Update(gt);
}

void MainGame::Draw(const GameTimer & gt)
{
	Renderer::Draw(gt);
}

void MainGame::RebuildFrameResouce()
{
	for (int i = 0; i < mFrameResources.size(); i++)
	{
		mFrameResources[i]->RebuildResource(md3dDevice.Get(), mAllRitems.size());
	}
	for (auto& item : mAllRitems)
	{
		item->NumFramesDirty = gNumFrameResources;
	}
}
