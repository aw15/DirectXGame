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
	virtual void OnResize() override; //â�� ũ�Ⱑ ���ϸ� �ĸ���ۿ� ���̽��ٽ� ���۸� ���� �����Ѵ�.
	virtual void Update(const GameTimer& gt) override;//�������Ӹ��� ��ü�� ���¸� ������Ʈ�Ѵ�.
	virtual void Draw(const GameTimer& gt) override;//�� �����Ӹ��� �ĸ���ۿ� ����� �׸���.



};

