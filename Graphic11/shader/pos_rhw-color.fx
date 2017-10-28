
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POSDIFFUSE VS( float4 Pos : POSITION
	, float4 Color : COLOR )
{
	VSOUT_POSDIFFUSE output = (VSOUT_POSDIFFUSE)0;
    output.Pos = mul( Pos, gWorld );
    output.Color = Color;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_POSDIFFUSE In ) : SV_Target
{
	return In.Color;
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

