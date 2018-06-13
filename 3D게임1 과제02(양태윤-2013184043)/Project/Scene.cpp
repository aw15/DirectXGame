//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

//extern vector<CGameObject*> m_objects;

CScene::CScene()
{
	m_pd3dGraphicsRootSignature = NULL;
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
}

void CScene::CreateShader(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_nShaders = 1;
	m_pShaders = new CInstancingShader[m_nShaders];
	////////////////////////////////////////////////////////////////
	m_pShaders[0].SetPlayer(m_pPlayer);
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].InstanceObjects(pd3dDevice, pd3dCommandList);//첫번째 인스턴스 생성
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	if (m_pShaders) delete[] m_pShaders;
}
void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].ReleaseUploadBuffers();
}
void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fireTime += fTimeElapsed;
	m_bossFireTime += fTimeElapsed;
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	}
	for (auto& bullet : m_bullets)
	{
		bullet->Animate(fTimeElapsed);
	}
	for (auto& bullet : m_bossBullets)
	{
		bullet->Animate(fTimeElapsed);
	}
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	return(pd3dGraphicsRootSignature);
}


ID3D12RootSignature *CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

void CScene::SpawnObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if(m_setFire)
	{
		CBullet* bullet = new CBullet(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		bullet->SetMovingDirection(m_pPlayer->GetUp());
		//bullet->SetPosition(m_pPlayer->GetPosition());
		bullet->m_xmf4x4World = m_pPlayer->m_xmf4x4World;
		XMMATRIX mtxRotate = XMMatrixRotationAxis({1,0,0},
			XMConvertToRadians(90));
		bullet->m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, bullet->m_xmf4x4World);
		bullet->SetMovingSpeed(100.5f);
		m_bullets.push_back(bullet);
		m_setFire = false;
	}
	if (m_bossFireTime > BOSSFIRECOOLTIME)
	{
		CBullet* bullet = new CBullet(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		auto toPlayer = Vector3::Subtract(m_pPlayer->GetPosition(), m_pBoss->GetPosition());
		bullet->SetMovingDirection(toPlayer);
		bullet->SetPosition(m_pBoss->GetPosition());

		toPlayer = Vector3::Normalize(toPlayer);
		auto normal = Vector3::Normalize(m_pBoss->GetLook());
		XMMATRIX mtxRotate = XMMatrixRotationAxis({ 0,1,0 },
			XMScalarACos(Vector3::DotProduct(normal, toPlayer)));
		bullet->m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, bullet->m_xmf4x4World);


		bullet->SetMovingSpeed(100.5f);
		bullet->SetColor({ 1,1,1,1 });
		m_bossBullets.push_back(bullet);
		m_bossFireTime = 0;
	}
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);
	BulletAndObjectCollide(pd3dDevice, pd3dCommandList);

}

void CScene::BulletAndObjectCollide(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	auto objectArray = m_pShaders[0].GetObjectArray();

		for (auto bullet : m_bullets)
		{
			if (m_pBoss->m_xmOOBB.Intersects(bullet->m_xmOOBB))
			{
				bullet->m_dead = true;
				m_pBoss->m_hp -= 1;
			}
			for (int i = 0; i < objectArray->size(); i++)
			{
				if ((*objectArray)[i]->m_tag == ENEMY && bullet->m_xmOOBB.Intersects((*objectArray)[i]->m_xmOOBB))
				{
					m_pShaders[0].CreateExplosiveObject(pd3dDevice, pd3dCommandList, (*objectArray)[i]->GetPosition());
					(*objectArray)[i]->m_dead = true;
					bullet->m_dead = true;
					if ((*objectArray)[i]->m_color.x == 1 && (*objectArray)[i]->m_color.y == 0&& (*objectArray)[i]->m_color.z == 0)
					{
						m_pPlayer->m_item += 1;
					}
				}
			}
			if (bullet->m_totalLifeTime > BULLETLIFETIME)
			{
				bullet->m_dead = true;
			}
		}

		for (auto bullet : m_bossBullets)
		{
			if (m_pPlayer->m_xmOOBB.Intersects(bullet->m_xmOOBB))
			{
				bullet->m_dead = true;
				m_pPlayer->m_hp -= 1;
			}
		}
}

void CScene::Restart()
{
	for (auto iter = m_bullets.begin(); iter != m_bullets.end();)
	{
		(*iter)->m_dead = true;
	}
	for (auto iter = m_bossBullets.begin(); iter != m_bossBullets.end();)
	{
		(*iter)->m_dead = true;
	}
	m_pShaders[0].Restart();
}



bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{

	if (pKeysBuffer[VK_SPACE] & 0xF0 && m_fireTime > FIRECOOLTIME)
	{
		m_setFire = true;
		m_fireTime = 0;
	}
	if (pKeysBuffer[0x52] & 0xF0 && m_pPlayer->m_item > 0)
	{
		cout << m_pPlayer->m_item;
		m_pShaders[0].SetExplode(true);
	}
	return(false);
}


void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
	for (auto bullet = m_bullets.begin() ; bullet!=m_bullets.end();)
	{
		(*bullet)->Render(pd3dCommandList, pCamera);
		if ((*bullet)->m_dead)
		{
			delete *bullet;
			bullet = m_bullets.erase(bullet);
		}
		else
		{
			bullet++;
		}
	}
	for (auto bullet = m_bossBullets.begin(); bullet != m_bossBullets.end();)
	{
		(*bullet)->Render(pd3dCommandList, pCamera);
		if ((*bullet)->m_dead)
		{
			delete *bullet;
			bullet = m_bossBullets.erase(bullet);
		}
		else
		{
			bullet++;
		}
	}
}
