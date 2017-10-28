
#define SHADOW_EPSILON 0.001f

#define Instancing		true
#define NotInstancing	false


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse	: register(t0);
Texture2D txBump	: register(t1);
Texture2D txShadow0	: register(t2);
Texture2D txShadow1	: register(t3);
Texture2D txShadow2	: register(t4);


SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samAnis : register(s1)
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow : register(s2)
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(1,1,1,1);
	ComparisonFunc = GREATER_EQUAL;
};


RasterizerState Depth
{
	DepthBias = 10000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};


cbuffer cbPerFrame : register( b0 )
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
	matrix gLightView[3];
	matrix gLightProj[3];
	matrix gLightTT;
	float3 gEyePosW;
}


cbuffer cbLight : register( b1 )
{
	float4 gLight_Ambient;
	float4 gLight_Diffuse;
	float4 gLight_Specular;
	float3 gLight_Direction;
	float3 gLight_PosW;
}


cbuffer cbMaterial : register( b2 )
{
	float4 gMtrl_Ambient;
	float4 gMtrl_Diffuse;
	float4 gMtrl_Specular;
	float4 gMtrl_Emissive;
	float gMtrl_Pow;
}


cbuffer cbPerFrameInstancing : register( b3 )
{
	matrix gWorldInst[100];
}


cbuffer cbClipPlane : register(b4)
{
	float4	gClipPlane;
}



//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
    float2 Tex : TEXCOORD0;
    float3 toEye : TEXCOORD1;
	float clip : SV_ClipDistance0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

    output.Pos = mul( Pos, mWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Normal = normalize( mul(Normal, (float3x3)mWorld) );
	output.Tangent = normalize(mul(Tangent, (float3x3)mWorld));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorld));
    output.Tex = Tex;
	output.clip = dot(mul(Pos, mWorld), gClipPlane);

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT In ) : SV_Target
{
	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	float4 color  = gLight_Ambient * gMtrl_Ambient
			+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L))
			+ gLight_Specular * gMtrl_Specular * pow( max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = color * txDiffuse.Sample(samLinear, In.Tex);
	return float4(Out.xyz, gMtrl_Diffuse.a);
}



//--------------------------------------------------------------------------------------
// Vertex Shader ShadowMap
//--------------------------------------------------------------------------------------
struct VS_SHADOW_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float2 Tex : TEXCOORD0;
	float4 TexShadow0 : TEXCOORD1;
	float4 TexShadow1 : TEXCOORD2;
	float4 TexShadow2 : TEXCOORD3;
	float3 toEye : TEXCOORD4;
	float4 Depth0 : TEXCOORD5;
	float2 Depth1 : TEXCOORD6;
	float clip : SV_ClipDistance0;
};

VS_SHADOW_OUTPUT VS_ShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Normal = normalize(mul(Normal, (float3x3)mWorld));
	output.Tangent = normalize(mul(Tangent, (float3x3)mWorld));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorld));
	output.Tex = Tex;

	matrix mLVP[3];
	matrix mVPT[3];

	float4 wPos = mul(Pos, mWorld);
	mLVP[0] = mul(gLightView[0], gLightProj[0]);
	mVPT[0] = mul(mLVP[0], gLightTT);
	mLVP[1] = mul(gLightView[1], gLightProj[1]);
	mVPT[1] = mul(mLVP[1], gLightTT);
	mLVP[2] = mul(gLightView[2], gLightProj[2]);
	mVPT[2] = mul(mLVP[2], gLightTT);

	output.TexShadow0 = mul(wPos, mVPT[0]);
	output.TexShadow1 = mul(wPos, mVPT[1]);
	output.TexShadow2 = mul(wPos, mVPT[2]);

	output.Depth0.xy = mul(wPos, mLVP[0]).zw;
	output.Depth0.zw = mul(wPos, mLVP[1]).zw;
	output.Depth1.xy = mul(wPos, mLVP[2]).zw;

	output.clip = dot(mul(Pos, mWorld), gClipPlane);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader ShadowMap
//--------------------------------------------------------------------------------------
float4 PS_ShadowMap(VS_SHADOW_OUTPUT In) : SV_Target
{
	float4 vTexCoords[3][9];
	float dx = 1.0f / 1024.0f;
	float depth0 = min(In.Depth0.x / In.Depth0.y, 1.0);
	float depth1 = min(In.Depth0.z / In.Depth0.w, 1.0);
	float depth2 = min(In.Depth1.x / In.Depth1.y, 1.0);

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
	// 7 6 8
	vTexCoords[0][0] = In.TexShadow0;
	vTexCoords[0][1] = In.TexShadow0 + float4(-dx, 0.0f, 0.0f, 0.0f);
	vTexCoords[0][2] = In.TexShadow0 + float4(dx, 0.0f, 0.0f, 0.0f);
	vTexCoords[0][3] = In.TexShadow0 + float4(0.0f, -dx, 0.0f, 0.0f);
	vTexCoords[0][6] = In.TexShadow0 + float4(0.0f, dx, 0.0f, 0.0f);
	vTexCoords[0][4] = In.TexShadow0 + float4(-dx, -dx, 0.0f, 0.0f);
	vTexCoords[0][5] = In.TexShadow0 + float4(dx, -dx, 0.0f, 0.0f);
	vTexCoords[0][7] = In.TexShadow0 + float4(-dx, dx, 0.0f, 0.0f);
	vTexCoords[0][8] = In.TexShadow0 + float4(dx, dx, 0.0f, 0.0f);

	vTexCoords[1][0] = In.TexShadow1;
	vTexCoords[1][1] = In.TexShadow1 + float4(-dx, 0.0f, 0.0f, 0.0f);
	vTexCoords[1][2] = In.TexShadow1 + float4(dx, 0.0f, 0.0f, 0.0f);
	vTexCoords[1][3] = In.TexShadow1 + float4(0.0f, -dx, 0.0f, 0.0f);
	vTexCoords[1][6] = In.TexShadow1 + float4(0.0f, dx, 0.0f, 0.0f);
	vTexCoords[1][4] = In.TexShadow1 + float4(-dx, -dx, 0.0f, 0.0f);
	vTexCoords[1][5] = In.TexShadow1 + float4(dx, -dx, 0.0f, 0.0f);
	vTexCoords[1][7] = In.TexShadow1 + float4(-dx, dx, 0.0f, 0.0f);
	vTexCoords[1][8] = In.TexShadow1 + float4(dx, dx, 0.0f, 0.0f);

	vTexCoords[2][0] = In.TexShadow2;
	vTexCoords[2][1] = In.TexShadow2 + float4(-dx, 0.0f, 0.0f, 0.0f);
	vTexCoords[2][2] = In.TexShadow2 + float4(dx, 0.0f, 0.0f, 0.0f);
	vTexCoords[2][3] = In.TexShadow2 + float4(0.0f, -dx, 0.0f, 0.0f);
	vTexCoords[2][6] = In.TexShadow2 + float4(0.0f, dx, 0.0f, 0.0f);
	vTexCoords[2][4] = In.TexShadow2 + float4(-dx, -dx, 0.0f, 0.0f);
	vTexCoords[2][5] = In.TexShadow2 + float4(dx, -dx, 0.0f, 0.0f);
	vTexCoords[2][7] = In.TexShadow2 + float4(-dx, dx, 0.0f, 0.0f);
	vTexCoords[2][8] = In.TexShadow2 + float4(dx, dx, 0.0f, 0.0f);

	const float S0 = (depth0 - SHADOW_EPSILON);
	const float S1 = (depth1 - SHADOW_EPSILON);
	const float S2 = (depth2 - SHADOW_EPSILON);

	float fShadowTerm = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		const float2 uv0 = vTexCoords[0][i].xy;
		const float2 uv1 = vTexCoords[1][i].xy;
		const float2 uv2 = vTexCoords[2][i].xy;
		const float D0 = txShadow0.SampleCmpLevelZero(samShadow, uv0, S0);
		const float D1 = txShadow1.SampleCmpLevelZero(samShadow, uv1, S1);
		const float D2 = txShadow2.SampleCmpLevelZero(samShadow, uv2, S2);

		fShadowTerm += (1 - saturate(D0 + D1 + D2));
	}
	fShadowTerm /= 9.0f;

	float4 bumpMap = txBump.Sample(samAnis, In.Tex);
	// Expand the range of the normal value from (0, +1) to (-1, +1). 
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	float3 bumpNormal = In.Normal + bumpMap.x * In.Tangent + bumpMap.y * In.Binormal;

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	const float3 L = -gLight_Direction;
	const float3 H = normalize(L + normalize(In.toEye));
	const float3 N = normalize(bumpNormal);

	const float lightV = max(0, dot(N, L));
	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * 0.1
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * 0.1
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * fShadowTerm * 0.9
		+ gLight_Specular * gMtrl_Specular * pow(max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = float4(color.xyz, gMtrl_Diffuse.w) * txDiffuse.Sample(samAnis, In.Tex);
	//Out = txBump.Sample(samAnis, In.Tex).rgba;
	//Out = txDiffuse.Sample(samAnis, In.Tex);
	return Out;
}


//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
struct VS_BUILDSHADOW_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Depth : TEXCOORD0;
};


VS_BUILDSHADOW_OUTPUT VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VS_BUILDSHADOW_OUTPUT output = (VS_BUILDSHADOW_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Depth.xy = output.Pos.zw;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader BuildShadowMap
//--------------------------------------------------------------------------------------
float4 PS_BuildShadowMap(
	VS_BUILDSHADOW_OUTPUT In
	) : SV_Target
{
	return In.Depth.x / In.Depth.y;
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 ShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_ShadowMap(NotInstancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ShadowMap()));
	}
}


technique11 BuildShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap(NotInstancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}


//---------------------------------------------------------------------------------
// Instancing

technique11 Unlit_Instancing
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(Instancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 ShadowMap_Instancing
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_ShadowMap(Instancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ShadowMap()));
	}
}


technique11 BuildShadowMap_Instancing
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap(Instancing)));
		SetGeometryShader(NULL);
 	        SetHullShader(NULL);
        	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}
