
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POS VS( float4 Pos : POSITION )
{
	VSOUT_POS output = (VSOUT_POS)0;
    output.Pos = mul( Pos, gWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
	output.PosH = output.Pos;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_POS In ) : SV_Target
{
	return gMtrl_Diffuse;
}


//--------------------------------------------------------------------------------------
float4 PS_Outline(VSOUT_POS In) : SV_Target
{
	float4 Out = gMtrl_Diffuse;

	float2 coords;
	coords.x = (In.PosH.x / In.PosH.w + 1) * 0.5f;
	coords.y = 1 - ((In.PosH.y / In.PosH.w + 1) * 0.5f);

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

	clip(fOutline - 0.000001f);
	return float4(0.8f, 0, 0, fOutline*2.5f);
}



//-----------------------------------------------------------------------
technique11 DepthTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
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
		SetVertexShader(CompileShader(vs_5_0, VS()));
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
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
      	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

