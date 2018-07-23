////////////////////////////////////////////////////////////////////////////////
// Filename: fire.vs
////////////////////////////////////////////////////////////////////////////////


#define MAX_TEXTURE 16

#define FIRE_NOISE 2
#define FIRE_ALPHA 3
#define FIRE       4

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

struct MaterialData
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float4x4 MatTransform;
    uint DiffuseMapIndex;
    uint MatPad0;
    uint MatPad1;
    uint MatPad2;
};


// An array of textures, which is only supported in shader model 5.1+.  Unlike Texture2DArray, the textures
// in this array can be different sizes and formats, making it more flexible than texture arrays.
Texture2D gDiffuseMap[MAX_TEXTURE] : register(t0);

// Put in space1, so the texture array does not overlap with these resources.  
// The texture array will occupy registers t0, t1, ..., t3 in space0. 
StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);


SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
    uint gMaterialIndex;
    uint gObjPad0;
    uint gObjPad1;
    uint gObjPad2;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;
    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
    /////////////////////////////////////FIRE////////////////////////////////////////////////////////

    //float2 distortion1;
    //float2 distortion2;
    //float2 distortion3;
    //float distortionScale;
    //float distortionBias;
    //float2 gPadding;
    //////////////////////////////////////////////////////////////////////////////////////////////// 
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
   	float2 texCoords1 : TEXCOORD1;
	float2 texCoords2 : TEXCOORD2;
	float2 texCoords3 : TEXCOORD3;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FireVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    float3 gScrollSpeeds = { 1.3f, 2.1f, 2.3f };
    float3 gScales = { 1.0f, 2.0f, 3.0f };

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, gWorld);
    output.position = mul(output.position, gView);
    output.position = mul(output.position, gProj);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

    // Compute texture coordinates for first noise texture using the first scale and upward scrolling speed values.
	output.texCoords1 = (input.tex * gScales.x);
    output.texCoords1.y = output.texCoords1.y + (gTotalTime * gScrollSpeeds.x);

    // Compute texture coordinates for second noise texture using the second scale and upward scrolling speed values.
	output.texCoords2 = (input.tex * gScales.y);
    output.texCoords2.y = output.texCoords2.y + (gTotalTime * gScrollSpeeds.y);

    // Compute texture coordinates for third noise texture using the third scale and upward scrolling speed values.
	output.texCoords3 = (input.tex * gScales.z);
    output.texCoords3.y = output.texCoords3.y + (gTotalTime * gScrollSpeeds.z);
	
    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FirePixelShader(PixelInputType input) : SV_TARGET
{
    float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;


    float2 distortion1 = {0.1f, 0.2f};
    float2 distortion2 = {0.1f, 0.3f};
    float2 distortion3 = {0.1f, 0.1f};
    float distortionScale = 0.8f;
    float distortionBias = 0.5f;

	// Sample the same noise texture using the three different texture coordinates to get three different noise scales.
    noise1 = gDiffuseMap[FIRE_NOISE].Sample(gsamLinearWrap, input.texCoords1);
    noise2 = gDiffuseMap[FIRE_NOISE].Sample(gsamLinearWrap, input.texCoords2);
    noise3 = gDiffuseMap[FIRE_NOISE].Sample(gsamLinearWrap, input.texCoords3);

	// Move the noise from the (0, 1) range to the (-1, +1) range.
    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;

	// Distort the three noise x and y coordinates by the three different distortion x and y values.
    noise1.xy = noise1.xy * distortion1.xy;
    noise2.xy = noise2.xy * distortion2.xy;
    noise3.xy = noise3.xy * distortion3.xy;

	// Combine all three distorted noise results into a single noise result.
    finalNoise = noise1 + noise2 + noise3;

	// Perturb the input texture Y coordinates by the distortion scale and bias values.  
	// The perturbation gets stronger as you move up the texture which creates the flame flickering at the top effect.
    perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;

	// Now create the perturbed and distorted texture sampling coordinates that will be used to sample the fire color texture.
    noiseCoords.xy = (finalNoise.xy * perturb) + input.tex.xy;

	// Sample the color from the fire texture using the perturbed and distorted texture sampling coordinates.
	// Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
    fireColor = gDiffuseMap[FIRE].Sample(gsamLinearClamp, noiseCoords.xy);

	// Sample the alpha value from the alpha texture using the perturbed and distorted texture sampling coordinates.
	// This will be used for transparency of the fire.
	// Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
    alphaColor = gDiffuseMap[FIRE_ALPHA].Sample(gsamLinearClamp, noiseCoords.xy);

	// Set the alpha blending of the fire to the perturbed and distored alpha texture value.
    fireColor.a = alphaColor;
	
    return fireColor;
}
