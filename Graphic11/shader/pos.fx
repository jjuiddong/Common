
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POS VS( float4 Pos : POSITION
			, uint instID : SV_InstanceID
			, uniform bool IsInstancing
)
{
	VSOUT_POS output = (VSOUT_POS)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	float4 PosW = mul(Pos, mWorld);
    //output.Pos = mul( Pos, mWorld);
    output.Pos = mul( PosW, gView );
    output.Pos = mul( output.Pos, gProjection );
	output.PosH = output.Pos;
	output.PosW = PosW;
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
float4 PS_Heightmap(VSOUT_POS In) : SV_Target
{
	return In.PosW.y / 500.f;
}


//--------------------------------------------------------------------------------------
float4 PS_Outline(VSOUT_POS In) : SV_Target
{
	float4 Out = gMtrl_Diffuse;
	const float fOutline = GetOutline(In.PosH);
	clip(fOutline - 0.000001f);
	return float4(1.f, 1.f, 1.f, fOutline*2.5f);
}



//-----------------------------------------------------------------------
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



technique11 Heightmap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Heightmap()));
	}
}


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

