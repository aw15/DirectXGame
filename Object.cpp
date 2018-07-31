#include"stdafx.h"
#include "Object.h"


void RenderItem::Update(float elapsedTime)
{
	mStandardBox->Transform(mBoundingBox, XMMatrixScaling(1.5, 1.5, 1.5)*XMMatrixTranslation(World._41, World._42, World._43));

}



void Player::Move(bool inverse)
{
	if (!inverse)
	{
		World._41 += mDir.x;
		World._43 += mDir.z;
	}
	else
	{
		World._41 -= mDir.x;
		World._43 -= mDir.z;
	}
	mStandardBox->Transform(mBoundingBox, XMLoadFloat4x4(&World));
}

void Player::Update(float elapsedTime)
{
	Move();
	mStandardBox->Transform(mBoundingBox, XMLoadFloat4x4(&World));
}

void Bomb::Update(float elapsedTime)
{
	mLifetime += elapsedTime;
}

void Fire::Update(float elapsedTime)
{
	mLifetime += elapsedTime;
	
	mStandardBox->Transform(mBoundingBox, XMMatrixScaling(1, 1, 1)*XMMatrixTranslation(World._41, World._42, World._43));
	//auto rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mRotation));

	//XMStoreFloat4x4(&World, XMMatrixScaling(mScale.x,mScale.y,mScale.z)*rotation);
	//World._41 = mPosition.x;
	//World._42 = mPosition.y;
	//World._43 = mPosition.z;

	//XMStoreFloat4x4(&TexTransform, XMMatrixScaling(1, 1, 1));
}
