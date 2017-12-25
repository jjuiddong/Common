//
// Tessellation Quad, 1 control point
//
#include "common.fx"

Texture2D txHeight : register(t8);

SamplerState SamplerLinearWrap : register(s4)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


cbuffer TessellationBuffer : register(b6)
{
	float gTessellationAmount;
	float2 gSize;
	float gLevel;
	float4 gEdgeLevel;
	float4 gUVs;
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
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 toEye : TEXCOORD1;
};



////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////

HullInputType main(
	float4 Pos : POSITION
	, float3 Norm : NORMAL
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
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 4> inputPatch
	, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = max(1, pow(2, abs(gLevel - gEdgeLevel.x)));
	output.edges[1] = max(1, pow(2, abs(gLevel - gEdgeLevel.y)));
	output.edges[2] = max(1, pow(2, abs(gLevel - gEdgeLevel.z)));
	output.edges[3] = max(1, pow(2, abs(gLevel - gEdgeLevel.w)));
	output.inside[0] = 1;
	output.inside[1] = 1;

	// Set the tessellation factor for tessallating inside the triangle.
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
[outputcontrolpoints(4)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 4> patch
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
	, const OutputPatch<HullOutputType, 4> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;

	float2 v1 = lerp(patch[0].pos, patch[1].pos, uv.x) * gSize;
	float2 v2 = lerp(patch[2].pos, patch[3].pos, uv.x) * gSize;
	vertexPosition.xz = lerp(v1, v2, uv.y);
	vertexPosition.y = 0.1f;

	float4 PosW = mul(float4(vertexPosition, 1.0f), gWorld);
	//PosW.y = abs((sin(PosW.x*0.02f) + sin(PosW.x*0.03f) + sin(PosW.x*0.01f)) * 10.f);// 0.1f;
	//PosW.y += abs(sin(PosW.x*0.01f) * 3.f);// 0.1f;

	//const float2 tex = float2(PosW.x / 4096.f, PosW.z / 4096.f);
	//float heightMap = txHeight.SampleLevel(samLinear, tex, 0).x;
	//float heightMap = txHeight.SampleLevel(SamplerLinearWrap, float2(0.5f,0.5f), 0).x;

	float dx = 0.01f;
	float y0 = (sin(PosW.x*0.02f) + sin(PosW.x*0.03f)) * 10.f;// 0.1f;
	float y1 = (sin((PosW.x*0.02f)+dx) + sin((PosW.x*0.03f)+dx)) * 10.f;// 0.1f;
	float3 normal = normalize(float3(dx, y1 - y0, 0));

	PosW.y = 0;// heightMap * 1000.f;// y0;

	output.pos = mul(PosW, gView);
	output.pos = mul(output.pos, gProjection);

	// Send the input color into the pixel shader.
	float u = lerp(gUVs.x, gUVs.z, uv.x);
	float v = lerp(gUVs.y, gUVs.w, uv.y);
	output.Normal = normal;
	output.Tex = float2(u,v);
	output.toEye = normalize(float4(gEyePosW, 1) - PosW).xyz;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PixelInputType input) : SV_TARGET
{
	return float4(0, 0, 0, 1); //input.color;
}


float4 PS_Light(PixelInputType input) : SV_TARGET
{
	float4 color = GetLightingColor(input.Normal, input.toEye, 1.f);
	float4 texColor = txDiffuse.Sample(samLinear, float2(input.Tex.x, input.Tex.y));
	//float4 texColor = txHeight.Sample(samLinear, float2(0.5f, 0.5f));
	//return float4(0, 0, 0, 1); //input.color;
	//return texColor;// 
	return float4(texColor.xyz, 1);
}



technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 Light
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader()));
		SetPixelShader(CompileShader(ps_5_0, PS_Light()));
	}
}
