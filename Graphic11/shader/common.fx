
#define SHADOW_EPSILON 0.0007f
#define SHADOW_EPSILON2 0.001f // pos-norm-tex
#define Instancing		true
#define NotInstancing	false
#define DepthMapSize		1024.f
#define DepthMapSize_Scaled (DepthMapSize - 500.f)

float4 g_outlineColor = float4(0.8f, 0.f, 0.f, 1.f);


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
	//AddressU = WRAP;
	//AddressV = WRAP;
	AddressU = Clamp;
	AddressV = Clamp;
	BorderColor = float4(1, 1, 1, 1);
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
	matrix gWorldInst[256];
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
	float4 PosH : TEXCORRD0;
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
	float4 PosH : TEXCORRD1;
	float3 toEye : TEXCOORD2;
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
	float2 Tex : TEXCOORD1;
	float4 Color : COLOR0;
	float4 PosH : TEXCOORD2;
	float3 toEye : TEXCOORD3;
};

struct VSOUT_SHADOW
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float4 Color : COLOR0;
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


//---------------------------------------------------------------------------
// Function

// return Shadow Value
float GetShadowPCF(float4 depthData0
	, float2 depthData1
	, float2 shadowUV0
	, float2 shadowUV1
	, float2 shadowUV2
)
{
	float2 vTexCoords[3][9];
	float dx = 1.0f / 1024.0f;
	float depth0 = min(depthData0.x / depthData0.y, 1.0);
	float depth1 = min(depthData0.z / depthData0.w, 1.0);
	float depth2 = min(depthData1.x / depthData1.y, 1.0);

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
	// 7 6 8
	vTexCoords[0][0] = shadowUV0;
	vTexCoords[0][1] = shadowUV0 + float2(-dx, 0.0f);
	vTexCoords[0][2] = shadowUV0 + float2(dx, 0.0f);
	vTexCoords[0][3] = shadowUV0 + float2(0.0f, -dx);
	vTexCoords[0][6] = shadowUV0 + float2(0.0f, dx);
	vTexCoords[0][4] = shadowUV0 + float2(-dx, -dx);
	vTexCoords[0][5] = shadowUV0 + float2(dx, -dx);
	vTexCoords[0][7] = shadowUV0 + float2(-dx, dx);
	vTexCoords[0][8] = shadowUV0 + float2(dx, dx);

	vTexCoords[1][0] = shadowUV1;
	vTexCoords[1][1] = shadowUV1 + float2(-dx, 0.0f);
	vTexCoords[1][2] = shadowUV1 + float2(dx, 0.0f);
	vTexCoords[1][3] = shadowUV1 + float2(0.0f, -dx);
	vTexCoords[1][6] = shadowUV1 + float2(0.0f, dx);
	vTexCoords[1][4] = shadowUV1 + float2(-dx, -dx);
	vTexCoords[1][5] = shadowUV1 + float2(dx, -dx);
	vTexCoords[1][7] = shadowUV1 + float2(-dx, dx);
	vTexCoords[1][8] = shadowUV1 + float2(dx, dx);

	vTexCoords[2][0] = shadowUV2;
	vTexCoords[2][1] = shadowUV2 + float2(-dx, 0.0f);
	vTexCoords[2][2] = shadowUV2 + float2(dx, 0.0f);
	vTexCoords[2][3] = shadowUV2 + float2(0.0f, -dx);
	vTexCoords[2][6] = shadowUV2 + float2(0.0f, dx);
	vTexCoords[2][4] = shadowUV2 + float2(-dx, -dx);
	vTexCoords[2][5] = shadowUV2 + float2(dx, -dx);
	vTexCoords[2][7] = shadowUV2 + float2(-dx, dx);
	vTexCoords[2][8] = shadowUV2 + float2(dx, dx);

	const float S0 = (depth0 - SHADOW_EPSILON2);
	const float S1 = (depth1 - SHADOW_EPSILON2);
	const float S2 = (depth2 - SHADOW_EPSILON2);

	float fShadowTerm = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		const float D0 = txShadow0.SampleCmpLevelZero(samShadow, vTexCoords[0][i].xy, S0);
		const float D1 = txShadow1.SampleCmpLevelZero(samShadow, vTexCoords[1][i].xy, S1);
		const float D2 = txShadow2.SampleCmpLevelZero(samShadow, vTexCoords[2][i].xy, S2);

		fShadowTerm += (1 - saturate(D0 + D1 + D2));
	}
	fShadowTerm /= 9.0f;

	return fShadowTerm;
}


// return Outline Value
float GetOutline(float4 PosH)
{
	float2 coords;
	coords.x = (PosH.x / PosH.w + 1) * 0.5f;
	coords.y = 1 - ((PosH.y / PosH.w + 1) * 0.5f);

	const float dx = 1.f / DepthMapSize_Scaled;
	float2 vTexCoords[9];
	vTexCoords[0] = coords;
	vTexCoords[1] = coords + float2(-dx, 0.0f);
	vTexCoords[2] = coords + float2(dx, 0.0f);
	vTexCoords[3] = coords + float2(0.0f, -dx);
	vTexCoords[6] = coords + float2(0.0f, dx);
	vTexCoords[4] = coords + float2(-dx, -dx);
	vTexCoords[5] = coords + float2(dx, -dx);
	vTexCoords[7] = coords + float2(-dx, dx);
	vTexCoords[8] = coords + float2(dx, dx);

	float fOutline = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		fOutline += txDepth.SampleCmpLevelZero(samDepth, vTexCoords[i], 1);
	}
	fOutline /= 9.0f;

	return fOutline;
}


// return Outline Color
float4 GetOutlineColor(float outlineVal)
{
	return float4(g_outlineColor.xyz, outlineVal*2.5f);
}


// return Lighting Color
float4 GetLightingColor(const float3 normal, const float3 toEye, const float shadowTerm)
{
	const float3 L = -gLight_Direction;
	const float3 H = normalize(L + normalize(toEye));
	const float3 N = normalize(normal);

	const float lightV = max(0, dot(N, L));
	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * 0.1f
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * 0.1f
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * shadowTerm * 0.9f
		+ gLight_Specular * gMtrl_Specular * pow(max(0, dot(N, H)), gMtrl_Pow);

	return color;
}
