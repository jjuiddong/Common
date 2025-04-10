//
// Tessellation Quad, 1 control point
//
#include "common.fx"


cbuffer TessellationBuffer : register(b6)
{
	float gTessellationAmount;
	float2 gSize;
	float gLevel;
	float4 gEdgeLevel;
};

struct HullInputType
{
	float2 pos : ORIGIN;
	float2 tex : TEXCOORD0;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;

	float2 pos : ORIGIN;
	float2 tex : TEXCOORD0;
};

struct HullOutputType
{
	float2 pos : ORIGIN;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};



////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////

HullInputType main(
	float4 Pos : POSITION
	, float2 Tex : TEXCOORD0
)
{
	HullInputType output;
	output.pos = Pos.xz;
	output.tex = Tex;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 1> inputPatch
	, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;
	output.edges[0] = 1;
	output.edges[1] = 1;
	output.edges[2] = 1;
	output.edges[3] = 1;
	output.inside[0] = 1;
	output.inside[1] = 1;
	output.pos = inputPatch[0].pos;
	output.tex = inputPatch[0].tex;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(1)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 1> patch
	, uint pointId : SV_OutputControlPointID
	, uint patchId : SV_PrimitiveID)
{
	HullOutputType output = (HullOutputType)0;
	output.pos = patch[pointId].pos;
	output.tex = patch[pointId].tex;
	return output;
}



////////////////////////////////////////////////////////////////////////////////
// Domain Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]

PixelInputType ColorDomainShader(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<HullOutputType, 1> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;

	vertexPosition.xz = input.pos + uv * gSize;
	vertexPosition.y = 0.1f;

	//vertexPosition.y = sin(uv.x);// 0.1f;
	//float3 oPos = mul(float4(vertexPosition, 1.0f), gWorld).xyz;
	//vertexPosition.y = (sin(oPos.x*0.02f) + sin(oPos.x*0.03f)) * 10.f;// 0.1f;
	//float2 v1 = lerp(patch[0].origin, patch[1].origin, uv.x);
	//float2 v2 = lerp(patch[2].origin, patch[3].origin, uv.x);
	//vertexPosition.xz = lerp(v1, v2, uv.y);
	//vertexPosition.y = 0.1f;

	output.pos = mul(float4(vertexPosition, 1.0f), gWorld);
	//output.pos = mul(output.pos, gView);
	//output.pos = mul(output.pos, gProjection);

	// Send the input color into the pixel shader.
	//output.color = float4(0,0,0,1);
	output.tex = uv;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	//float4 texColor = txDiffuse.Sample(samLinear, input.tex);
	return float4(0, 0, 0, 1); //input.color;
	//return texColor;// 
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader()));
		SetPixelShader(CompileShader(ps_5_0, ColorPixelShader()));
	}
}

