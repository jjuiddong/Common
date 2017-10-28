
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_TEXDIFF VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float4 Color : COLOR )
{
	VSOUT_TEXDIFF output = (VSOUT_TEXDIFF)0;
    float4 PosW = mul( Pos, gWorld );
    output.Pos = mul(PosW, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Normal = normalize( mul(Normal, (float3x3)gWorld) );
    output.Tex = Tex;
	output.PosH = output.Pos;
	output.toEye = normalize(float4(gEyePosW, 1) - PosW).xyz;
    output.Color = Color;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_TEXDIFF In ) : SV_Target
{
	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	float4 color  = gLight_Ambient * gMtrl_Ambient
			+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L))
			+ gLight_Specular * gMtrl_Specular * pow( max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = txDiffuse.Sample(samAnis, In.Tex);
	//return float4(Out.x, Out.y, Out.z, 1);
	return Out;
}



//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
VSOUT_BUILDSHADOW VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float4 Color : COLOR)
{
	VSOUT_BUILDSHADOW output = (VSOUT_BUILDSHADOW)0;
	output.Pos = mul(Pos, gWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	return output;
}


//------------------------------------------------------------------------
// Techinique

technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
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
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 BuildShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap()));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
       	SetDomainShader(NULL);
		SetPixelShader(NULL);
	}
}

