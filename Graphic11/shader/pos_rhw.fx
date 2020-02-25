
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POSDIFFUSE VS( float4 Pos : POSITION
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_POSDIFFUSE output = (VSOUT_POSDIFFUSE)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;
	const float4 color = IsInstancing ? gDiffuseInst[instID] : gMtrl_Diffuse;

    output.Pos = mul( Pos, mWorld );
    output.Color = color;
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
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
		SetGeometryShader(NULL);
 	    SetHullShader(NULL);
        SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 Light
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

