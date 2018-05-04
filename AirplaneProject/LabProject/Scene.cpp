#include "stdafx.h"
#include "Scene.h"




CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_CONTROL:
				{
					m_pPlayer->Fire();
					break;
				}
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
				/*	CExplosiveObject *pExplosiveObject = (CExplosiveObject *)pObjects[int(wParam - '1')];
					pExplosiveObject->m_bBlowingUp = true;
					break;*/
				}
				case 'Z':
					if (m_pPlayer->m_itemCount > 0)
					{
						for (auto& cube : m_objects)
						{
							CExplosiveObject *pExplosiveObject = (CExplosiveObject *)cube;
							pExplosiveObject->m_bBlowingUp = true;
						}
						m_pPlayer->m_itemCount-=2;
						
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void CScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 1100.0f;
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);
	pWallCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.3f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);

	pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	pObjectCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));



}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	for (auto iter = m_objects.begin();iter!=m_objects.end();)
	{
		delete *iter;
		iter = m_objects.erase(iter);
	}
	m_objects.clear();


	if (m_pWallsObject) delete m_pWallsObject;
}

void CScene::CheckObjectByObjectCollisions()
{
	for (auto& cube:m_objects) cube->m_pObjectCollided = NULL;
	CGameObject* pickObject=nullptr;
	float maxdist = 0;

	for (int i = 0; i < m_objects.size(); i++)
	{
		XMVECTOR origin,direction;
		float dist=0;
		//m_ray.x *= m_objects[i]->m_xmf4x4World._41;
		//m_ray.y *= m_objects[i]->m_xmf4x4World._42;
		//m_ray.z *= m_objects[i]->m_xmf4x4World._43;

		//origin = DirectX::XMLoadFloat3(&m_pPlayer->m_pCamera->m_xmf3Position);
		//direction = DirectX::XMLoadFloat3(&m_ray);

		//if (m_objects[i]->m_xmOOBB.Intersects(origin, direction, dist))
		//{
		//	if (maxdist < abs(dist))
		//	{
		//		pickObject = m_objects[i];
		//	}
		//}
		//

		////if (dist == 0)
		////{
		////	m_objects[i]->SetColor(RGB(255, 255, 0));
		////}

		for (auto& bullet:m_pPlayer->m_bullets)//총알과의 충돌체크.
		{
			if (m_objects[i]->m_xmOOBB.Intersects(bullet->m_xmOOBB))
			{
				CExplosiveObject *pExplosiveObject = (CExplosiveObject *)m_objects[i];
				pExplosiveObject->m_bBlowingUp = true;
				bullet->alive = false;
				if (m_objects[i]->m_dwColor == RGB(255, 0, 0))
				{
					m_pPlayer->m_itemCount += 1;
				}
			}
		}
		if (m_objects[i]->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB))
		{
			m_pPlayer->SetColor(m_objects[i]->m_dwColor);
		}
		

		for (int j = (i + 1); j < m_objects.size(); j++)
		{
			if (m_objects[i]->m_xmOOBB.Intersects(m_objects[j]->m_xmOOBB))
			{
				m_objects[i]->m_pObjectCollided = m_objects[j];
				m_objects[j]->m_pObjectCollided = m_objects[i];
			}
		}
	}
	for (int i = 0; i <m_objects.size(); i++)
	{
		if (m_objects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_objects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_objects[i]->m_fMovingSpeed;
			m_objects[i]->m_xmf3MovingDirection = m_objects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_objects[i]->m_fMovingSpeed = m_objects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_objects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_objects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_objects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_objects[i]->m_pObjectCollided = NULL;
		}
	}

	if (pickObject)
	{
		pickObject->SetColor(RGB(255, 255, 0));
	}

}

void CScene::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_objects.size(); i++)
	{
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_objects[i]->m_xmOOBB);
		switch (containType)
		{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_objects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_objects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_objects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 6; j++)
				{
					PlaneIntersectionType intersectType = m_objects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_objects[i]->m_xmf3MovingDirection), xmvNormal);
					XMStoreFloat3(&m_objects[i]->m_xmf3MovingDirection, xmvReflect);
				}
				break;
			}
			case CONTAINS:
				break;
		}
	}
	ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_pPlayer->m_xmOOBB);
	switch (containType)
	{
	case DISJOINT:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == BACK)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pPlayer->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&m_pPlayer->m_xmf3MovingDirection, xmvReflect);
		}
		break;
	}
	case INTERSECTS:
	{
		int nPlaneIndex = -1;
		for (int j = 0; j < 6; j++)
		{
			PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
			if (intersectType == INTERSECTING)
			{
				nPlaneIndex = j;
				break;
			}
		}
		if (nPlaneIndex != -1)
		{
			XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
			XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pPlayer->m_xmf3MovingDirection), xmvNormal);
			XMStoreFloat3(&m_pPlayer->m_xmf3MovingDirection, xmvReflect);
			switch (nPlaneIndex)
			{
			case 0:
				m_pPlayer->Move(DIR_RIGHT, 1.3f);
				break;
			case 1:
				m_pPlayer->Move(DIR_LEFT, 1.3f);
				break;
			case 2:
				m_pPlayer->Move(DIR_UP, 1.3f);
				break;
			case 3:
				m_pPlayer->Move(DIR_DOWN, 1.3f);
				break;
			}
		}
		break;
	}
	case CONTAINS:
		break;
	}

}

void CScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);
	/*if (m_pWallsObject->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) == DISJOINT) m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);*/

	for (int i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Animate(fElapsedTime);
	}

	CheckObjectByWallCollisions();

	CheckObjectByObjectCollisions();

	spawnTime += fElapsedTime;
	redSpawnTime += fElapsedTime;

	if (spawnTime > 1)
	{
		auto pObjects = new CGameObject;

		pObjects = new CExplosiveObject();
		pObjects->SetMesh(pObjectCubeMesh);
		pObjects->SetColor(m_cubeColors[rand()%3]);

		pObjects->SetPosition(m_pPlayer->m_xmf3Position.x+rand()%20-10,m_pPlayer->m_xmf3Position.y+rand() % 20 - 10, m_pPlayer->m_xmf3Position.z+100);
		pObjects->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
		pObjects->SetRotationSpeed(90.0f);
		pObjects->SetMovingDirection(XMFLOAT3(0, 0, -1));
		pObjects->SetMovingSpeed(10.5f);

		m_objects.push_back(pObjects);


		m_pBoss->Fire();
		spawnTime = 0;
	}
	if(redSpawnTime > 10)
	{
		auto pObjects = new CGameObject;

		pObjects = new CExplosiveObject();
		pObjects->SetMesh(pObjectCubeMesh);
		pObjects->SetColor(RGB(255,0,0));
		pObjects->SetPosition(m_pPlayer->m_xmf3Position.x + rand() % 20 - 10, m_pPlayer->m_xmf3Position.y + rand() % 20 - 10, m_pPlayer->m_xmf3Position.z + 100);
		pObjects->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
		pObjects->SetRotationSpeed(90.0f);
		pObjects->SetMovingDirection(XMFLOAT3( 0,0,-1 ));
		pObjects->SetMovingSpeed(20.5f);

		m_objects.push_back(pObjects);
		redSpawnTime = 0;
	}


}

void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < m_objects.size(); i++) m_objects[i]->Render(hDCFrameBuffer, pCamera);
}
