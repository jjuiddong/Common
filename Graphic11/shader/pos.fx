
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
	const float fOutline = GetOutline(In.PosH);
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

