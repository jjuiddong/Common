
#define SHADOW_EPSILON 0.0007f
#define SHADOW_EPSILON2 0.001f // pos-norm-tex
#define Instancing		true
#define NotInstancing	false
#define DepthMapSize		1024.f
#define DepthMapSize_Scaled (DepthMapSize - 500.f)

//--------------------------------------------------------------------------------------
// Texture
//--------------------------------------------------------------------------------------
Texture2D txDiffuse	: register(t0);
Texture2D txBump	: register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txShadow0	: register(t4);
Texture2D txShadow1	: register(t5);
Texture2D txShadow2	: register(t6);
Texture2D txDepth : register(t7);


//--------------------------------------------------------------------------------------
// SamplerState
//--------------------------------------------------------------------------------------
SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow : register(s1)
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(1, 1, 1, 1);
	ComparisonFunc = GREATER_EQUAL;
};

SamplerComparisonState samDepth : register(s2)
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(0, 0, 0, 0);
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samAnis : register(s3)
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};




//--------------------------------------------------------------------------------------
// RasterizerState
//--------------------------------------------------------------------------------------
RasterizerState Depth
{
	DepthBias = 10000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

RasterizerState NoCull
{
	CullMode = NONE;
};

DepthStencilState DepthNormal
{
	DepthFunc = LESS_EQUAL;
	DepthEnable = TRUE;
};

DepthStencilState NoDepthStencil
{
	DepthEnable = FALSE;
};


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register(b0)
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
	matrix gLightView[3];
	matrix gLightProj[3];
	matrix gLightTT;
	float3 gEyePosW;
}


cbuffer cbLight : register(b1)
{
	float4 gLight_Ambient;
	float4 gLight_Diffuse;
	float4 gLight_Specular;
	float3 gLight_Direction;
	float3 gLight_PosW;
}


cbuffer cbMaetrial : register(b2)
{
	float4 gMtrl_Ambient;
	float4 gMtrl_Diffuse;
	float4 gMtrl_Specular;
	float4 gMtrl_Emissive;
	float gMtrl_Pow;
}

cbuffer cbPerFrameInstancing : register(b3)
{
	matrix gWorldInst[100];
}

cbuffer cbClipPlane : register(b4)
{
	float4	gClipPlane;
}

cbuffer cbSkinning : register(b5)
{
	matrix gPalette[64];
}


//--------------------------------------------------------------------------------------
struct VSOUT_POS
{
	float4 Pos : SV_POSITION;
};

struct VSOUT_POSDIFFUSE
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

struct VSOUT_POSDIFFUSE_TEX
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
	float2 Tex : TEXCOORD0;
};


struct VSOUT_POSTEX
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD1;
	float clip : SV_ClipDistance0;
};

struct VSOUT_POSNORM
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float3 toEye : TEXCOORD1;
	float clip : SV_ClipDistance0;
};

struct VSOUT_NORMAL
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float4 PosH : TEXCORRD1;
	float3 toEye : TEXCOORD2;
};

struct VSOUT_DIFFUSE
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float4 Color : COLOR0;
	float4 PosH : TEXCORRD1;
	float3 toEye : TEXCOORD2;
};

struct VSOUT_TEX
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float2 Tex : TEXCOORD1;
	float4 PosH : TEXCOORD2;
	float3 toEye : TEXCOORD3;
	float clip : SV_ClipDistance0;
};

struct VSOUT_TEXDIFF
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float4 Color : COLOR0;
	float2 Tex : TEXCOORD1;
	float4 PosH : TEXCOORD2;
	float3 toEye : TEXCOORD3;
};

struct VSOUT_SHADOW
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 TexShadow0 : TEXCOORD1;
	float4 TexShadow1 : TEXCOORD2;
	float4 TexShadow2 : TEXCOORD3;
	float4 PosH : TEXCOORD4;
	float3 toEye : TEXCOORD5;
	float4 Depth0 : TEXCOORD6;
	float2 Depth1 : TEXCOORD7;
	float clip : SV_ClipDistance0;
};

struct VSOUT_BUILDSHADOW
{
	float4 Pos : SV_POSITION;
	float2 Depth : TEXCOORD0;
};

struct VSOUT_BUMP
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float3 Tangent : TEXCOORD1;
	float3 Binormal : TEXCOORD2;
	float2 Tex : TEXCOORD3;
	float4 PosH : TEXCOORD4;
	float3 toEye : TEXCOORD5;
	float clip : SV_ClipDistance0;
};

struct VSOUT_BUMPSHADOW
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float3 Tangent : TEXCOORD1;
	float3 Binormal : TEXCOORD2;
	float2 Tex : TEXCOORD3;
	float4 TexShadow0 : TEXCOORD4;
	float4 TexShadow1 : TEXCOORD5;
	float4 TexShadow2 : TEXCOORD6;
	float4 PosH : TEXCOORD7;
	float3 toEye : TEXCOORD8;
	float4 Depth0 : TEXCOORD9;
	float2 Depth1 : TEXCOORD10;
	float clip : SV_ClipDistance0;
};
