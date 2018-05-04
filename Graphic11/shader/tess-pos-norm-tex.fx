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
	float4 gTUVs;
	float4 gHUVs;
};


struct HullInputType
{
	float2 Pos : ORIGIN;
	float2 Tex : TEXCOORD0;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;

	float2 Pos : ORIGIN;
	float2 Tex : TEXCOORD0;
};

struct HullOutputType
{
	float2 Pos : ORIGIN;
	float2 Tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 toEye : TEXCOORD1;
	float3 PosW : TEXCOORD2;
};

struct PixelInputType2
{
	float4 pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex1 : TEXCOORD0;
	float2 Tex2 : TEXCOORD1;
	float3 toEye : TEXCOORD2;
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
	output.Pos = Pos.xz;
	output.Tex = Tex;
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
	//output.edges[0] = max(1, pow(2, (6 - max(0, gLevel - gEdgeLevel.x))));
	//output.edges[1] = max(1, pow(2, (6 - max(0, gLevel - gEdgeLevel.y))));
	//output.edges[2] = max(1, pow(2, (6 - max(0, gLevel - gEdgeLevel.z))));
	//output.edges[3] = max(1, pow(2, (6 - max(0, gLevel - gEdgeLevel.w))));

	output.edges[0] = 64;
	output.edges[1] = 64;
	output.edges[2] = 64;
	output.edges[3] = 64;
	output.inside[0] = 64;
	output.inside[1] = 64;

	// Set the tessellation factor for tessallating inside the triangle.
	output.Pos = inputPatch[0].Pos;
	output.Tex = inputPatch[0].Tex;

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
	output.Pos = patch[pointId].Pos;
	output.Tex = patch[pointId].Tex;
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

	const float ac1 = 1.0f / 64.0f;
	const float2 ac2 = gSize * (2.0f / 64.0f);

	float u2 = saturate((uv.x - ac1) * (64.f/62.f));
	float v2 = saturate((uv.y - ac1)* (64.f / 62.f));

	float2 p1 = lerp(patch[0].Pos, patch[1].Pos, uv.x) * gSize;
	float2 p2 = lerp(patch[2].Pos, patch[3].Pos, uv.x) * gSize;
	float2 p12 = lerp(patch[0].Pos, patch[1].Pos, u2) * gSize;
	float2 p22 = lerp(patch[2].Pos, patch[3].Pos, u2) * gSize;

	float2 pos1 = lerp(p1, p2, uv.y);
	float2 pos2 = lerp(p12, p22, v2);
	
	vertexPosition.xz = float2(min(pos2.x, gSize.x), min(pos2.y, (gSize.y)));
	//vertexPosition.xz = pos2;
	vertexPosition.y = 0.1f;

	float4 PosW = mul(float4(vertexPosition, 1.0f), gWorld);
	const float2 tex = float2(lerp(gHUVs.x, gHUVs.z, uv.x), lerp(gHUVs.y, gHUVs.w, uv.y));
	float heightMap = txHeight.SampleLevel(samPoint, tex, 0).x;

	PosW.y = (heightMap - 0.1f) * 2500.f;

	output.Pos = mul(PosW, gView);
	output.Pos = mul(output.Pos, gProjection);

	// Send the input color into the pixel shader.
	float2 tuv = float2((uv.x - ac1) * (64.f / 62.f), (uv.y - ac1) * (64.f / 62.f));
	float u = lerp(gTUVs.x, gTUVs.z, tuv.x);
	float v = lerp(gTUVs.y, gTUVs.w, tuv.y);

	//float u = lerp(gTUVs.x, gTUVs.z, uv.x);
	//float v = lerp(gTUVs.y, gTUVs.w, uv.y);
	output.Normal = float3(0, 1, 0);
	output.Tex = float2(u, v);
	output.toEye = normalize(gEyePosW - PosW).xyz;
	output.PosW = PosW.xyz;




	//float2 v1 = lerp(patch[0].Pos, patch[1].Pos, uv.x) * gSize;
	//float2 v2 = lerp(patch[2].Pos, patch[3].Pos, uv.x) * gSize;
	//vertexPosition.xz = lerp(v1, v2, uv.y);
	//vertexPosition.y = 0.1f;

	//float4 PosW = mul(float4(vertexPosition, 1.0f), gWorld);
	//const float2 tex = float2(lerp(gHUVs.x, gHUVs.z, uv.x), lerp(gHUVs.y, gHUVs.w, uv.y));
	//float heightMap = txHeight.SampleLevel(samPoint, tex, 0).x;

	//PosW.y = heightMap * 500.f;

	//output.Pos = mul(PosW, gView);
	//output.Pos = mul(output.Pos, gProjection);

	//// Send the input color into the pixel shader.
	//float u = lerp(gTUVs.x, gTUVs.z, uv.x);
	//float v = lerp(gTUVs.y, gTUVs.w, uv.y);
	//output.Normal = float3(0,1,0);
	//output.Tex = float2(u,v);
	//output.toEye = normalize(gEyePosW - PosW).xyz;
	//output.PosW = PosW.xyz;

	return output;
}


[domain("quad")]

PixelInputType ColorDomainShader_Heightmap(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<HullOutputType, 4> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;

	float2 v1 = lerp(patch[0].Pos, patch[1].Pos, uv.x) * gSize;
	float2 v2 = lerp(patch[2].Pos, patch[3].Pos, uv.x) * gSize;
	vertexPosition.xz = lerp(v1, v2, uv.y);
	vertexPosition.y = 0.1f;

	float4 PosW = mul(float4(vertexPosition, 1.0f), gWorld);
	//PosW.y = abs((sin(PosW.x*0.02f) + sin(PosW.x*0.03f) + sin(PosW.x*0.01f)) * 10.f);// 0.1f;
	//PosW.y += abs(sin(PosW.x*0.01f) * 3.f);// 0.1f;

	//const float2 tex = float2(PosW.x / 4096.f, PosW.z / 4096.f);
	//float heightMap = txHeight.SampleLevel(samPoint, tex, 0).x;
	//float heightMap = txHeight.SampleLevel(SamplerLinearWrap, float2(0.5f,0.5f), 0).x;

	float dx = 0.01f;
	float y0 = (sin(PosW.x*0.02f) + sin(PosW.x*0.03f)) * 10.f;// 0.1f;
	float y1 = (sin((PosW.x*0.02f) + dx) + sin((PosW.x*0.03f) + dx)) * 10.f;// 0.1f;
	float3 normal = normalize(float3(dx, y1 - y0, 0));

	PosW.y = 0;// heightMap * 1000.f;// y0;

	output.Pos = mul(PosW, gView);
	output.Pos = mul(output.Pos, gProjection);

	// Send the input color into the pixel shader.
	//float u = lerp(gTUVs.x, gTUVs.z, uv.x);
	//float v = lerp(gTUVs.y, gTUVs.w, uv.y);
	float u = lerp(gHUVs.x, gHUVs.z, uv.x);
	float v = lerp(gHUVs.y, gHUVs.w, uv.y);
	output.Normal = normal;
	output.Tex = float2(u, v);
	output.toEye = normalize(gEyePosW - PosW).xyz;

	return output;
}



[domain("quad")]

PixelInputType2 ColorDomainShader_LightHeightmap(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<HullOutputType, 4> patch)
{
	float3 vertexPosition;
	PixelInputType2 output = (PixelInputType2)0;

	float2 p1 = lerp(patch[0].Pos, patch[1].Pos, uv.x) * gSize;
	float2 p2 = lerp(patch[2].Pos, patch[3].Pos, uv.x) * gSize;
	vertexPosition.xz = lerp(p1, p2, uv.y);
	vertexPosition.y = 0.1f;

	float4 PosW = mul(float4(vertexPosition, 1.0f), gWorld);
	//PosW.y = abs((sin(PosW.x*0.02f) + sin(PosW.x*0.03f) + sin(PosW.x*0.01f)) * 10.f);// 0.1f;
	//PosW.y += abs(sin(PosW.x*0.01f) * 3.f);// 0.1f;

	//const float2 tex = float2(PosW.x / 4096.f, PosW.z / 4096.f);
	//float heightMap = txHeight.SampleLevel(samPoint, tex, 0).x;
	//float heightMap = txHeight.SampleLevel(SamplerLinearWrap, float2(0.5f,0.5f), 0).x;

	float dx = 0.01f;
	float y0 = (sin(PosW.x*0.02f) + sin(PosW.x*0.03f)) * 10.f;// 0.1f;
	float y1 = (sin((PosW.x*0.02f) + dx) + sin((PosW.x*0.03f) + dx)) * 10.f;// 0.1f;
	float3 normal = normalize(float3(dx, y1 - y0, 0));

	PosW.y = 0;// heightMap * 1000.f;// y0;

	output.pos = mul(PosW, gView);
	output.pos = mul(output.pos, gProjection);

	// Send the input color into the pixel shader.
	float u1 = lerp(gTUVs.x, gTUVs.z, uv.x);
	float v1 = lerp(gTUVs.y, gTUVs.w, uv.y);
	float u2 = lerp(gHUVs.x, gHUVs.z, uv.x);
	float v2 = lerp(gHUVs.y, gHUVs.w, uv.y);
	output.Normal = normal;
	output.Tex1 = float2(u1, v1);
	output.Tex2 = float2(u2, v2);
	output.toEye = normalize(gEyePosW - PosW).xyz;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PixelInputType input) : SV_TARGET
{
	float4 texColor = txDiffuse.Sample(samLinear, float2(input.Tex.x, input.Tex.y));
	return float4(texColor.xyz, 1);
}


float4 PS_Light(PixelInputType input) : SV_TARGET
{
	float4 color = GetLightingColor(input.Normal, input.toEye, 1.f);
	float4 texColor = txDiffuse.Sample(samLinear, float2(input.Tex.x, input.Tex.y));
	float4 Out = color * texColor;

	// Fog
	const float d = distance(input.PosW, gEyePosW.xyz);
	Out = lerp(Out, float4(gFogColor.rgb, 1), saturate(d*gFogColor.w));
	return float4(Out.xyz, 1);
}


float4 PS_NoTexture(PixelInputType input) : SV_TARGET
{
	float4 color = GetLightingColor(input.Normal, input.toEye, 1.f);
	float4 Out = color;

	// Fog
	const float d = distance(input.PosW, gEyePosW.xyz);
	Out = lerp(Out, float4(gFogColor.rgb, 1), saturate(d*gFogColor.w));
	return float4(Out.xyz, 1);
}


float4 PS_Heightmap(PixelInputType input) : SV_TARGET
{
	float4 color = GetLightingColor(input.Normal, input.toEye, 1.f);
	float4 texColor = txHeight.Sample(samPoint, float2(input.Tex.x, input.Tex.y));
	return float4(texColor.xyz, 1) * 100.f;
}


float4 PS_LightHeightmap(PixelInputType2 input) : SV_TARGET
{
	float4 color = GetLightingColor(input.Normal, input.toEye, 1.f);
	float4 texColor1 = txDiffuse.Sample(samLinear, float2(input.Tex1.x, input.Tex1.y));
	float4 texColor2 = txHeight.Sample(samPoint, float2(input.Tex2.x, input.Tex2.y));
	return float4(texColor1.xyz, 1) + float4(texColor2.x, 0, 0, 0)*100.f;
}



////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
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


technique11 NoTexture
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader()));
		SetPixelShader(CompileShader(ps_5_0, PS_NoTexture()));
	}
}


technique11 Heightmap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader_Heightmap()));
		SetPixelShader(CompileShader(ps_5_0, PS_Heightmap()));
	}
}


technique11 Light_Heightmap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader_LightHeightmap()));
		SetPixelShader(CompileShader(ps_5_0, PS_LightHeightmap()));
	}
}

