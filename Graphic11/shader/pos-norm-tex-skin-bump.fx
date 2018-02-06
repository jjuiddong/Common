
#include "common.fx"


//--------------------------------------------------------------------------------------
// Skinning Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_BUMP VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint4 BlendId : BLENDINDICES0
	, float4 BlendWeight : BLENDWEIGHT0
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_BUMP output = (VSOUT_BUMP)0;
    const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

    float3 p = {0,0,0};
    float3 n = {0,0,0};

    p += mul(Pos, gPalette[ BlendId.x]).xyz * BlendWeight.x;
    p += mul(Pos, gPalette[ BlendId.y]).xyz * BlendWeight.y;
    p += mul(Pos, gPalette[ BlendId.z]).xyz * BlendWeight.z;
    p += mul(Pos, gPalette[ BlendId.w]).xyz * BlendWeight.w;

    n += mul(Normal, (float3x3)gPalette[ BlendId.x]) * BlendWeight.x;
    n += mul(Normal, (float3x3)gPalette[ BlendId.y]) * BlendWeight.y;
    n += mul(Normal, (float3x3)gPalette[ BlendId.z]) * BlendWeight.z;
    n += mul(Normal, (float3x3)gPalette[ BlendId.w]) * BlendWeight.w;

    n = normalize(n);

	float4 PosW = mul(float4(p, 1), mWorld);
    output.Pos = mul( PosW, gView );
    output.Pos = mul( output.Pos, gProjection );

    output.Normal = normalize( mul(n, (float3x3)mWorld) );
	output.Tangent = normalize(mul(Tangent, (float3x3)mWorld));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorld));
    output.Tex = Tex;
	output.toEye = normalize(gEyePosW - PosW).xyz;
	output.PosH = output.Pos;
    output.clip = dot(mul(Pos, mWorld), gClipPlane);

    return output;
}


//--------------------------------------------------------------------------------------
// Skinning Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_BUMP In ) : SV_Target
{
	// 스타2 노멀맵은 rgba 순서로 저장된게 아니라. bgxr 형태로 저장되어 있다.
	// 그래서 노멀값을 제대로 가져오려먼 agr 값을 가져와야 rgb즉 xyz 순서대로 
	// 가져오게 된다.
	// http://blog.naver.com/scahp/130109083917
	// http://forum.xentax.com/viewtopic.php?f=16&t=6119&start=15
	float4 bumpMap = txBump.Sample(samAnis, In.Tex).agrb;
	// Expand the range of the normal value from (0, +1) to (-1, +1). 
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	float3 bumpNormal = In.Normal + bumpMap.x * In.Tangent + bumpMap.y * In.Binormal;

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(bumpNormal);

	float4 color  = gLight_Ambient * gMtrl_Ambient
			+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L))
			;

	float4 Out = color * txDiffuse.Sample(samLinear, In.Tex)
		+ gLight_Specular * pow( max(0, dot(N,H)), gMtrl_Pow*10) * txSpecular.Sample(samAnis, In.Tex)
		+ float4(0.8,0,0,1) * txEmissive.Sample(samAnis, In.Tex).a
		;

	return float4(Out.xyz, gMtrl_Diffuse.a);
}


//--------------------------------------------------------------------------------------
// Skinning Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_Outline(VSOUT_BUMP In) : SV_Target
{
	const float fOutline = GetOutline(In.PosH);
	clip(fOutline - 0.000001f);
	return GetOutlineColor(fOutline);
}


//--------------------------------------------------------------------------------------
// Vertex Shader ShadowMap
//--------------------------------------------------------------------------------------
VSOUT_BUMPSHADOW VS_ShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint4 BlendId : BLENDINDICES0
	, float4 BlendWeight : BLENDWEIGHT0
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_BUMPSHADOW output = (VSOUT_BUMPSHADOW)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	float3 p = {0,0,0};
	float3 n = {0,0,0};

	p += mul(Pos, gPalette[ BlendId.x]).xyz * BlendWeight.x;
	p += mul(Pos, gPalette[ BlendId.y]).xyz * BlendWeight.y;
	p += mul(Pos, gPalette[ BlendId.z]).xyz * BlendWeight.z;
	p += mul(Pos, gPalette[ BlendId.w]).xyz * BlendWeight.w;

	n += mul(Normal, (float3x3)gPalette[ BlendId.x]) * BlendWeight.x;
	n += mul(Normal, (float3x3)gPalette[ BlendId.y]) * BlendWeight.y;
	n += mul(Normal, (float3x3)gPalette[ BlendId.z]) * BlendWeight.z;
	n += mul(Normal, (float3x3)gPalette[ BlendId.w]) * BlendWeight.w;

	n = normalize(n);

	float4 PosW = mul(float4(p, 1), mWorld);
	output.Pos = mul(PosW, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Normal = normalize(mul(n, (float3x3)mWorld));
	output.Tangent = normalize(mul(Tangent, (float3x3)mWorld));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorld));
	output.Tex = Tex;
	output.toEye = normalize(gEyePosW - PosW).xyz;
	output.PosH = output.Pos;

	matrix mLVP[3];
	matrix mVPT[3];

	mLVP[0] = mul(gLightView[0], gLightProj[0]);
	mVPT[0] = mul(mLVP[0], gLightTT);
	mLVP[1] = mul(gLightView[1], gLightProj[1]);
	mVPT[1] = mul(mLVP[1], gLightTT);
	mLVP[2] = mul(gLightView[2], gLightProj[2]);
	mVPT[2] = mul(mLVP[2], gLightTT);

	output.TexShadow0 = mul(PosW, mVPT[0]);
	output.TexShadow1 = mul(PosW, mVPT[1]);
	output.TexShadow2 = mul(PosW, mVPT[2]);

	output.Depth0.xy = mul(PosW, mLVP[0]).zw;
	output.Depth0.zw = mul(PosW, mLVP[1]).zw;
	output.Depth1.xy = mul(PosW, mLVP[2]).zw;

	output.clip = dot(mul(Pos, mWorld), gClipPlane);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader ShadowMap
//--------------------------------------------------------------------------------------
float4 PS_ShadowMap(VSOUT_BUMPSHADOW In) : SV_Target
{
	// 스타2 노멀맵은 rgba 순서로 저장된게 아니라. bgxr 형태로 저장되어 있다.
	// 그래서 노멀값을 제대로 가져오려먼 agr 값을 가져와야 rgb즉 xyz 순서대로 
	// 가져오게 된다.
	// http://blog.naver.com/scahp/130109083917
	// http://forum.xentax.com/viewtopic.php?f=16&t=6119&start=15
	float4 bumpMap = txBump.Sample(samAnis, In.Tex).agrb;
	// Expand the range of the normal value from (0, +1) to (-1, +1). 
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	float3 bumpNormal = In.Normal + bumpMap.x * In.Tangent + bumpMap.y * In.Binormal;

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(bumpNormal);

	const float fShadowTerm = GetShadowPCF(In.Depth0, In.Depth1
		, In.TexShadow0.xy, In.TexShadow1.xy, In.TexShadow2.xy);

	const float lightV = max(0, dot(N, L));
	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * 0.1
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * 0.1
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * fShadowTerm * 0.9;

	float4 Out = color * txDiffuse.Sample(samAnis, In.Tex)
		+ gLight_Specular * pow(max(0, dot(N, H)), gMtrl_Pow) * txSpecular.Sample(samAnis, In.Tex)
		+ float4(0.8, 0, 0, 1) * txEmissive.Sample(samAnis, In.Tex).a
		;

	return float4(Out.xyz, gMtrl_Diffuse.a);
}


//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
VSOUT_BUILDSHADOW VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint4 BlendId : BLENDINDICES0
	, float4 BlendWeight : BLENDWEIGHT0
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_BUILDSHADOW output = (VSOUT_BUILDSHADOW)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	float3 p = {0,0,0};

	p += mul(Pos, gPalette[ BlendId.x]).xyz * BlendWeight.x;
	p += mul(Pos, gPalette[ BlendId.y]).xyz * BlendWeight.y;
	p += mul(Pos, gPalette[ BlendId.z]).xyz * BlendWeight.z;
	p += mul(Pos, gPalette[ BlendId.w]).xyz * BlendWeight.w;

	output.Pos = mul(float4(p,1), mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Depth.xy = output.Pos.zw;

	return output;
}


float4 PS_BuildShadowMap(VSOUT_BUILDSHADOW In) : SV_Target
{
	clip(gMtrl_Diffuse.a - 0.5f);
	return float4(1, 1, 1, 1);
}



//---------------------------------------------------------------------------------
// Technique

technique11 DepthTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(NULL);
	}
}


technique11 Outline
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Outline()));
	}
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
