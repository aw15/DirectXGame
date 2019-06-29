#pragma once
#include"Common/d3dApp.h"



class MyGame : public D3DApp
{
public:
	MyGame(HINSTANCE hInstance);
	virtual ~MyGame();
	MyGame(const MyGame& rhs) = delete;
	MyGame& operator=(const MyGame& rhs) = delete;

	virtual bool Initialize() override;


private:
	virtual void OnResize() override; //창의 크기가 변하면 후면버퍼와 깊이스텐실 버퍼를 새로 생성한다.
	virtual void Update(const GameTimer& gt) override;//매프레임마다 객체의 상태를 업데이트한다.
	virtual void Draw(const GameTimer& gt) override;//매 프레임마다 후면버퍼에 장면을 그린다.



};

