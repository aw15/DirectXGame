#include "stdafx.h"
#include "Shader.h"




CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i])
			m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}

//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}
//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}
//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다. 
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR
pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
UINT nCompileFlags = 0;
#if defined(_DEBUG)
nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile,
	nCompileFlags, 0, ppd3dShaderBlob, NULL);
D3D12_SHADER_BYTECODE d3dShaderByteCode;
d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
return(d3dShaderByteCode);
}

//그래픽스 파이프라인 상태 객체를 생성한다.
void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
d3dPipelineStateDesc.BlendState = CreateBlendState();
d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
d3dPipelineStateDesc.InputLayout = CreateInputLayout();
d3dPipelineStateDesc.SampleMask = UINT_MAX;
d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
d3dPipelineStateDesc.NumRenderTargets = 1;
d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
d3dPipelineStateDesc.SampleDesc.Count = 1;
d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
	__uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[]
d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
}
void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4
	*pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void CShader::ReleaseShaderVariables()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}
void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}


CDiffusedShader::CDiffusedShader()
{
}
CDiffusedShader::~CDiffusedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CDiffusedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CDiffusedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}
//CDiffusedShader 클래스의 CreateShader() 함수를 다음과 같이 정의한다.
void CDiffusedShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}


////////////////////////////////////////////////////////////////////////////////////////
CObjectsShader::CObjectsShader()
{
}
CObjectsShader::~CObjectsShader()
{
}
D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
}

void CObjectsShader::ReleaseObjects()
{
	for (auto iter =  m_objects.begin();iter!= m_objects.end();)
	{
		delete *iter;
		iter=m_objects.erase(iter);
	}
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_objects.size(); j++)
	{
		m_objects[j]->Animate(fTimeElapsed);
	}
}

void CObjectsShader::ReleaseUploadBuffers()
{
	for(auto& object: m_objects)
		object->ReleaseUploadBuffers();
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_objects.size(); j++)
	{
		if (m_objects[j])
		{
			m_objects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}



CInstancingShader::CInstancingShader()
{

}
CInstancingShader::~CInstancingShader()
{
}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	//정점 정보를 위한 입력 원소이다. 
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	//인스턴싱 정보를 위한 입력 원소이다. 
	pd3dInputElementDescs[2] = { "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,
	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[3] = { "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[4] = { "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,
	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[5] = { "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,
	D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	pd3dInputElementDescs[6] = { "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
	64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1",
		ppd3dShaderBlob));
}
void CInstancingShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	//인스턴스 정보를 저장할 정점 버퍼를 업로드 힙 유형으로 생성한다.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
	sizeof(VS_VB_INSTANCE) * m_objects.size(), D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
		//정점 버퍼에 대한 뷰를 생성한다. 
	m_d3dInstancingBufferView.BufferLocation =
	m_pd3dcbGameObjects->GetGPUVirtualAddress();
	m_d3dInstancingBufferView.StrideInBytes = sizeof(VS_VB_INSTANCE);
	m_d3dInstancingBufferView.SizeInBytes = sizeof(VS_VB_INSTANCE) * m_objects.size();
}

void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}

//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다. 
void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int j = 0; j < m_objects.size(); j++)
	{
		m_pcbMappedGameObjects[j].m_xmcColor = m_objects[j]->m_color;
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_objects[j]->m_xmf4x4World)));
	}
}


void CInstancingShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	CRotatingObject *pObjects = NULL;
	XMFLOAT3 playerPos = { 0,0,0 };
	if (m_pPlayer)
		playerPos = m_pPlayer->GetPosition();


	if (m_spawnTime > SPAWN)
	{
		pObjects = new CRotatingObject();
		pObjects->SetPosition(playerPos.x + rand() % 20 - 10, playerPos.y + rand() % 20 - 10, playerPos.z + 100);
		pObjects->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		pObjects->SetRotationSpeed(120.0f);
		auto movingDirection = Vector3::Subtract(m_pPlayer->GetPosition(), pObjects->GetPosition());
		pObjects->SetMovingDirection(movingDirection);

		int colorIndex = rand() % 3;
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
		//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다. 
		m_spawnTime = 0;
	}
	if (m_redSpawnTime > REDSPAWN)
	{
		auto pObjects = new CGameObject;

		pObjects = new CRotatingObject();
		pObjects->SetPosition(playerPos.x + rand() % 20 - 10, playerPos.y + rand() % 20 - 10, playerPos.z + 100);
		pObjects->SetRotationAxis(XMFLOAT3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX));
		auto movingDirection = Vector3::Subtract(m_pPlayer->GetPosition(), pObjects->GetPosition());
		pObjects->SetMovingDirection(movingDirection);
		pObjects->SetRotationSpeed(90.0f);
		pObjects->SetMovingSpeed(15.5f);
		pObjects->SetColor(m_cubeColors[3]);

		m_objects.push_back(pObjects);
		m_redSpawnTime = 0;
	}
	//인스턴싱을 위한 정점 버퍼와 뷰를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	PlayerObjectCollideCheck(pd3dDevice, pd3dCommandList);

	if (m_explodeAll)
	{
		for (auto& object : m_objects)
		{
			if (object->m_tag == ENEMY)
			{
				object->m_dead = true;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			CreateExplosiveObject(pd3dDevice, pd3dCommandList, m_pPlayer->GetPosition());
		}

		m_pPlayer->m_item -= 1;
		m_explodeAll = false;
	}

}

void CInstancingShader::InstanceObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	float fxPitch = 12.0f + rand() % 100;
	float fyPitch = 12.0f + rand() % 100;
	float fzPitch = 12.0f + rand() % 100;
	CRotatingObject *pRotatingObject = NULL;
	pRotatingObject = new CRotatingObject();
	XMFLOAT3 playerPos = { 0,0,0 };
	if(m_pPlayer)
		playerPos=m_pPlayer->GetPosition();

	pRotatingObject->SetPosition(10000,11100,10000);
	pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pRotatingObject->SetRotationSpeed(120.0f);
	pRotatingObject->m_tag = INSTANCE;

	//auto movingDirection = Vector3::Subtract(m_pPlayer->GetPosition(), pRotatingObject->GetPosition());
	//pRotatingObject->SetMovingDirection(movingDirection);
	//pRotatingObject->SetMovingSpeed(5.5f);
	//pRotatingObject->SetColor(m_cubeColors[1]);


	m_objects.push_back(pRotatingObject);
	//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다. 
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		12.0f, 12.0f, 12.0f);
	m_objects[0]->SetMesh(pCubeMesh);
	//인스턴싱을 위한 정점 버퍼와 뷰를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CInstancingShader::AnimateObjects(float fTimeElapsed)
{
	m_spawnTime += fTimeElapsed;
	m_redSpawnTime += fTimeElapsed;

	for (int j = 0; j < m_objects.size(); j++)
	{
		auto movingDirection = Vector3::Subtract(m_pPlayer->GetPosition(), m_objects[j]->GetPosition());
		m_objects[j]->SetMovingDirection(movingDirection);

		m_objects[j]->Animate(fTimeElapsed);
	}

	

}

void CInstancingShader::PlayerObjectCollideCheck(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{

	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->m_tag==ENEMY&&m_pPlayer->m_xmOOBB.Intersects(m_objects[i]->m_xmOOBB))//&&m_objects[i]->m_coolTime<=0)
		{
			CreateExplosiveObject(pd3dDevice, pd3dCommandList, m_pPlayer->GetPosition());
			//_objects[i]->m_coolTime = COLLIDECOOLTIME;
			m_objects[i]->m_dead = true;
			m_pPlayer->m_hp -= 1;
		}
	}
}

void CInstancingShader::ObjectCollideCheck()
{
	for (int j = 0; j < m_objects.size(); j++)
	{
		for (int i = j + 1; i < m_objects.size(); i++)
		{
			if (m_objects[i]->m_xmOOBB.Intersects(m_objects[j]->m_xmOOBB))
			{
				m_objects[i]->m_pObjectCollided = m_objects[j];
				m_objects[j]->m_pObjectCollided = m_objects[i];
			}
		}
	}
}

void CInstancingShader::CreateExplosiveObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, XMFLOAT3 position)
{
	for (int i = 0; i < 50; i++)
	{
		CExplosiveObject *pObjects = NULL;
		XMFLOAT3 playerPos = { 0,0,0 };

		pObjects = new CExplosiveObject(XMFLOAT3(rand() / (float)RAND_MAX * 2 - 1, rand() / (float)RAND_MAX * 2 - 1, rand() / (float)RAND_MAX * 2 - 1));
		pObjects->SetPosition(position.x, position.y, position.z);
		pObjects->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		pObjects->SetRotationSpeed(120.0f);
		pObjects->SetMovingSpeed(105.5f);
		int colorIndex = rand() % 3;
		pObjects->SetColor(m_cubeColors[colorIndex]);
		m_objects.push_back(pObjects);
		m_objects.back()->m_tag = EXPLOSIVE;
		//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다. 
	}
	//인스턴싱을 위한 정점 버퍼와 뷰를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CInstancingShader::Restart()
{
	for (auto iter = m_objects.begin() + 1; iter != m_objects.end();)
	{
		(*iter)->m_dead = true;
	}
}

void CInstancingShader::ReleaseObjects()
{
}

void CInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	CObjectsShader::Render(pd3dCommandList, pCamera);
	//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
	UpdateShaderVariables(pd3dCommandList);
	//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
	m_objects[0]->Render(pd3dCommandList, pCamera, m_objects.size(),
	m_d3dInstancingBufferView);

	//cout << m_objects.size() << endl;
	for (auto iter = m_objects.begin()+1; iter != m_objects.end();)
	{
		if ((*iter)->m_dead)
		{
			delete *iter;
			iter = m_objects.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}