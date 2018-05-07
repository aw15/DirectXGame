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
					if (m_pPlayer->m_coolTime > 1)
					{
						m_pPlayer->Fire();
						m_pPlayer->m_coolTime = 0;
					}
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

	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 1000.0f;
	CWallMesh *pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);
	pWallCubeMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

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
		//플레이어와 오브젝트 충돌체크
		if (m_objects[i]->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB)&&m_pPlayer->m_invincibilityTime<0)
		{
			m_pPlayer->SetColor(m_objects[i]->m_dwColor);
			m_pPlayer->m_hp -= 1;
			m_pPlayer->m_invincibilityTime = 1;
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
	//플레이어와 벽간에 콜리전 체크
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
			m_pPlayer->SetPosition(0, 0, m_pPlayer->m_xmf3Position.z);
			m_pPlayer->SetMovingDirection(XMFLOAT3{ 0,1,0 });
			//m_pPlayer->SetColor(RGB(0, 0, 0));
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
			m_pPlayer->SetPosition(0, 0, m_pPlayer->m_xmf3Position.z);
			m_pPlayer->SetMovingDirection(XMFLOAT3{ 0,1,0 });
			//m_pPlayer->SetColor(RGB(0, 0, 0));
		}
		break;
	}
	case CONTAINS:
		break;
	}

}

void CScene::BulletCollisions()
{
	for (auto& bullet : m_pPlayer->m_bullets)//총알과의 충돌체크.
	{
		if (m_pBoss->m_xmOOBB.Intersects(bullet->m_xmOOBB))
		{
			m_pBoss->m_hp -= 1;
		}
	}

	for (auto& bullet : m_pBoss->m_bullets)//총알과의 충돌체크.
	{
		if (m_pPlayer->m_xmOOBB.Intersects(bullet->m_xmOOBB))
		{
			m_pPlayer->m_hp -= 1;
		}
	}
}

void CScene::Restart()
{

		m_pBoss->Dead();
		m_pPlayer->Dead();
		for (auto iter = m_objects.begin(); iter != m_objects.end();)
		{
			delete *iter;
			iter = m_objects.erase(iter);
		}
		m_pPlayer->SetPosition(0.0f, 0.0f, -500.0f);
		m_pPlayer->SetColor(RGB(0, 0, 255));
		m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
		m_pPlayer->m_hp = 3;
		m_pBoss->SetPosition(10.0f, 0.0f, 500.0f);
		m_pBoss->Rotate(0, 180, 0);
		m_pBoss->SetColor(RGB(255, 100, 100));
		m_pBoss->m_hp = 3;


		spawnTime = 0;
		redSpawnTime = 0;
}

void CScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);
	/*if (m_pWallsObject->m_xmOOBB.Contains(XMLoadFloat3(&m_pPlayer->m_xmf3Position)) == DISJOINT) m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);*/
	int backCount = 0;

	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->GetPosition().z < m_pPlayer->GetPosition().z)
		{
			backCount++;
		}

		m_objects[i]->Animate(fElapsedTime);
		if (m_objects[i]->m_spawnTime > 2)
		{
			XMFLOAT3 dir;
			dir.x = m_pPlayer->m_xmf3Position.x -  m_objects[i]->GetPosition().x;
			dir.y = m_pPlayer->m_xmf3Position.y - m_objects[i]->GetPosition().y;
			dir.z = m_pPlayer->m_xmf3Position.z - m_objects[i]->GetPosition().z;
			m_objects[i]->SetMovingDirection(dir);
		}
		else
		{
			m_objects[i]->m_spawnTime += fElapsedTime;
		}
	}

	if (backCount > 0)
	{
		m_pPlayer->SetColor(RGB(255, 0, 255));//뒤에 있다면 분홍색으로
	}
	CheckObjectByWallCollisions();

	CheckObjectByObjectCollisions();
	BulletCollisions();

	spawnTime += fElapsedTime;
	redSpawnTime += fElapsedTime;

	if (spawnTime > 1)
	{
		auto pObjects = new CGameObject;

		pObjects = new CExplosiveObject();
		pObjects->SetMesh(pObjectCubeMesh);
		int colorIndex = rand() % 3;
		pObjects->SetPosition(m_pPlayer->m_xmf3Position.x + rand() % 20 - 10, m_pPlayer->m_xmf3Position.y + rand() % 20 - 10, m_pPlayer->m_xmf3Position.z + 100);
		pObjects->SetRotationAxis(XMFLOAT3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX));
		pObjects->SetMovingDirection(XMFLOAT3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX));
		pObjects->SetColor(m_cubeColors[colorIndex]);
		switch (colorIndex)
		{
		case 0:
			pObjects->SetRotationSpeed(120.0f);
			pObjects->SetMovingSpeed(5.5f);
		case 1:
			pObjects->SetRotationSpeed(90.0f);
			pObjects->SetMovingSpeed(10.5f);
		case 2:
			pObjects->SetRotationSpeed(60.0f);
			pObjects->SetMovingSpeed(15.5f);
			break;
		}
		m_objects.push_back(pObjects);
		spawnTime = 0;
	}
	if(redSpawnTime > 10)
	{
		auto pObjects = new CGameObject;

		pObjects = new CExplosiveObject();
		pObjects->SetMesh(pObjectCubeMesh);
		pObjects->SetColor(RGB(255,0,0));
		pObjects->SetPosition(m_pPlayer->m_xmf3Position.x + rand() % 20 - 10, m_pPlayer->m_xmf3Position.y + rand() % 20 - 10, m_pPlayer->m_xmf3Position.z + 100);
		pObjects->SetRotationAxis(XMFLOAT3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX));
		pObjects->SetMovingDirection(XMFLOAT3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX));
		pObjects->SetRotationSpeed(90.0f);
		pObjects->SetMovingSpeed(30.5f);

		m_objects.push_back(pObjects);
		redSpawnTime = 0;
	}

	

}

void CScene::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < m_objects.size(); i++) m_objects[i]->Render(hDCFrameBuffer, pCamera);
}
