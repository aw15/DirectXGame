#pragma once
#include"Renderer.h"
#include<iostream>
extern const int gNumFrameResources;

struct Player : public RenderItem
{
	void Walk(float amount)
	{
		World._43 += amount;
	}
	void Strafe(float amount)
	{
		World._41 += amount;
	}
	float m_speed = 10;
};


class MainGame : public Renderer
{
public:
	MainGame(HINSTANCE hInstance);
	~MainGame();
	virtual bool Initialize() override;
	void BuildMap();
	virtual void OnKeyboardInput(const GameTimer& gt) override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;
	void RebuildFrameResouce();
private:
	Player* m_player1;
	Player* m_player2;
	int objIndex = 0;
	bool isCreate = false;


};

