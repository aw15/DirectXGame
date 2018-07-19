#include"stdafx.h"
#include "Object.h"


void RenderItem::Update(float elapsedTime)
{
	m_standardBox->Transform(m_boundingBox, XMLoadFloat4x4(&World));
}



void Player::Move(bool inverse)
{
	if (!inverse)
	{
		World._41 += m_dir.x;
		World._43 += m_dir.z;
	}
	else
	{
		World._41 -= m_dir.x;
		World._43 -= m_dir.z;
	}
	m_standardBox->Transform(m_boundingBox, XMLoadFloat4x4(&World));
}

void Player::Update(float elapsedTime)
{
	Move();
}
