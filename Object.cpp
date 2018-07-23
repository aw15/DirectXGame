#include"stdafx.h"
#include "Object.h"


void RenderItem::Update(float elapsedTime)
{
	mStandardBox->Transform(mBoundingBox, XMLoadFloat4x4(&World));
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
}
