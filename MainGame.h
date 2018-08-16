#pragma once
#include"Renderer.h"
#include"Parser.h"

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
	void CreateObject(const SORT category,const char* materialName = "default", const XMFLOAT3& position = {0,0,0}, const XMFLOAT3& scale = { 1,1,1 }, const UINT playerID = PLAYER1);
	void DestroyItem(int index, std::vector<RenderItem*>& items);
	void RebuildFrameResouce();
private:
	Player* mPlayer[2];
	int		mObjectConstantCount = 0;
	
};

