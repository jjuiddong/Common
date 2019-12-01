//
// Tessellation
// Render Box(Quad) from 1 control point
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
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct HullOutputType
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float4 posW : TEXCOORD0;
	float2 tex : TEXCOORD1;
};



////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////

HullInputType VS(
	float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float4 Color : COLOR
)
{
	HullInputType output;
	output.pos = Pos.xyz;
	output.tex = Tex;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType PatchConstantFunction(InputPatch<HullInputType, 1> inputPatch
	, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	const float tessFactor = gTessellationAmount;
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
[outputtopology("triangle_cw")]
[outputcontrolpoints(1)]
[patchconstantfunc("PatchConstantFunction")]

HullOutputType HS(InputPatch<HullInputType, 1> patch
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

PixelInputType DS(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<HullOutputType, 1> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;
	vertexPosition = input.pos;

	// rotate to Camera Position
	float4 posW = mul(float4(input.pos, 1.0f), gWorld);
	float3 dir = normalize(posW - gEyePosW).xyz;
	float3 right = normalize(cross(dir, float3(0, 1, 0)));
	float3 up = normalize(cross(right, dir));

	output.pos = posW;
	output.pos.xyz += right * uv.x * gSize.x;
	output.pos.xyz += up * uv.y * gSize.y;
	output.posW = output.pos; // world coordinate 

	output.pos = mul(output.pos, gView);
	output.pos = mul(output.pos, gProjection);
	output.tex = input.tex;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PixelInputType input) : SV_TARGET
{
	float4 texColor = txDiffuse.Sample(samLinear, input.tex);
	//return float4(1, 1, 1, 1); //input.color;
	//return texColor;
	return float4(0, 0, 0, 1);
	//return input.posW.y / 500.f;

	return float4(1, 1, 1, 0.5);
	//return float4(
	//	float3(gMtrl_Diffuse.xyz)*0.3f * saturate(input.posW.y * 300)
	//	+ gMtrl_Diffuse.xyz * (input.posW.y/300)
	//	, gMtrl_Diffuse.w);
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

