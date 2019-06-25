#pragma once
#include "Common/d3dApp.h"
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"
#include "Common/Camera.h"
#include "FrameResource.h"

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;
	virtual void Move(bool inverse = false) {}
	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;

	//Collision
	BoundingBox			mBoundingBox;
	BoundingBox*		mStandardBox;
	UINT				mTag;

	float				mSpeed = 10;
	XMFLOAT3			mDir = { 0,0,0 }; 
	XMFLOAT3			mPosition = { 0,0,0 };
	XMFLOAT3			mRotation = { 0,0,0 };
	XMFLOAT3			mScale = { 10,10,10 };

	virtual void Update(float elapsedTime);
	XMFLOAT3 GetPosition3f() { return { World._41,World._42,World._43 }; }
	XMVECTOR GetPosition() { return XMLoadFloat3(&XMFLOAT3{ World._41,World._42,World._43 }); }
	XMMATRIX GetWorldMatrix() { return XMLoadFloat4x4(&World); }
};


struct Player : public RenderItem
{
	void Walk(float amount)
	{
		mDir.z += amount * mSpeed;
	}
	void Strafe(float amount)
	{
		mDir.x +=  amount * mSpeed;
	}
	void Move(bool inverse = false);
	void Update(float elapsedTime);
};

struct Bomb :public RenderItem
{
private:
	float	mLifetime = 0;
	int		mplayerID;
public:
	void Update(float elapsedTime);
	inline float GetLifetime() { return mLifetime; }
	inline void SetPlayerID(const int ID) { mplayerID = ID; }
};

struct Fire :public RenderItem
{
private:
	float	mLifetime = 0;
public:
	void Update(float elapsedTime);
	inline float GetLifetime() { return mLifetime; }
};

