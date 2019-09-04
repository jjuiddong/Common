
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POSDIFFUSE_TEX VS( float4 Pos : POSITION
	, float2 Tex : TEXCOORD0
	, float4 Color : COLOR
 )
{
	VSOUT_POSDIFFUSE_TEX output = (VSOUT_POSDIFFUSE_TEX)0;
    output.Pos = mul( Pos, gWorld );
    output.Color = Color;
	output.Tex = Tex;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_POSDIFFUSE_TEX In ) : SV_Target
{
	//return txDiffuse.Sample( samLinear, In.Tex );
	//return float4(1,0,0,1);
	return txDiffuse.Sample(samLinear, float2(gEyePosW.x, gEyePosW.y));
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

