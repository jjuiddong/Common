
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_DIFFUSE VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float4 Color : COLOR
	, uniform bool IsInstancing
	 )
{
	VSOUT_DIFFUSE output = (VSOUT_DIFFUSE)0;
	float4 PosW = mul(Pos, gWorld);
    output.Pos = mul(PosW, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Normal = normalize( mul(Normal, (float3x3)gWorld) );
	output.PosH = output.Pos;
	output.toEye = normalize(float4(gEyePosW, 1) - PosW).xyz;
    output.Color = Color;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_DIFFUSE In ) : SV_Target
{
	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	const float lightV = max(0, dot(N, L));
	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * 0.1
		+ gLight_Diffuse * gMtrl_Diffuse * lightV * 0.1
		+ gLight_Diffuse * gMtrl_Diffuse * lightV
		+ gLight_Specular * gMtrl_Specular * pow(max(0, dot(N, H)), gMtrl_Pow);

	float4 Out = color * In.Color;
	return Out;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_Outline(VSOUT_DIFFUSE In) : SV_Target
{
	float3 L = -gLight_Direction;
	float3 H = normalize(L + normalize(In.toEye));
	float3 N = normalize(In.Normal);

	float4 color = gLight_Ambient * gMtrl_Ambient
		+ gLight_Diffuse * gMtrl_Diffuse * max(0, dot(N,L))
		+ gLight_Specular * gMtrl_Specular * pow(max(0, dot(N,H)), gMtrl_Pow);

	float4 Out = color * In.Color;

	float2 coords;
	coords.x = (In.PosH.x / In.PosH.w + 1) * 0.5f;
	coords.y = 1 - ((In.PosH.y / In.PosH.w + 1) * 0.5f);

	const float dx = 1 / 400.f;
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

	Out += float4(1, 0, 0, 1) * fOutline;
	return float4(Out.xyz, gMtrl_Diffuse.a);
}


//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
VSOUT_BUILDSHADOW VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float4 Color : COLOR
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_BUILDSHADOW output = (VSOUT_BUILDSHADOW)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Depth.xy = output.Pos.zw;

	return output;
}



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
        SetVertexShader( CompileShader( vs_5_0, VS(NotInstancing) ) );
	SetGeometryShader( NULL );
        SetHullShader(NULL);
      	SetDomainShader(NULL);
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}


technique11 ShadowMap
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


technique11 BuildShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap(NotInstancing)));
		SetGeometryShader(NULL);
 	    SetHullShader(NULL);
        SetDomainShader(NULL);
		SetPixelShader(NULL);
	}
}


