#pragma once
#include"Renderer.h"
#include<iostream>
extern const int gNumFrameResources;



class MainGame : public Renderer
{
public:
	MainGame(HINSTANCE hInstance);
	~MainGame();
	virtual bool Initialize() override;
	void BuildMap();
	void OnKeyboardInput(const GameTimer& gt) override;
	void Update(const GameTimer& gt) override;
	void Draw(const GameTimer& gt) override;
	void CreateBomb(int playerID=PLAYER1);
	void CreateFire(XMFLOAT3& position);
	void DestroyItem(int index, std::vector<RenderItem*>& items);
	void RebuildFrameResouce();
private:
	Player* m_player1;
	Player* m_player2;
	int		m_objectConstantCount = 0;




};

