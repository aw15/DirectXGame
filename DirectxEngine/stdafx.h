#pragma once
#include <iostream>
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include<algorithm>
#include<queue>
#include<map>
#include"common/MathHelper.h"
#include "common/d3dUtil.h"
#include "common/GameTimer.h"

#define TEST(s) std::cout<<s<<std::endl
// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;



#define BOX 0x01
#define PLAYER 0x02
#define BOMB 0x03

#define INVERSE true

#define PLAYER1 0
#define PLAYER2 1

#define MAX_TEXTURE 16

#define EXPLOSIVE_COOLTIME 3

#define ROOT_SIGNITURE_SLOT 5




enum SORT
{
	player=0,
	box,
	bomb,
	fire,
	boundary,
	count
};

#define PLAYER_MODEL "player"
#define PLAYER_MODEL2 "player2"