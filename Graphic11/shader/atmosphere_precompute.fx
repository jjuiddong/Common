
typedef float scalar;
typedef float3 scalar3;

#include "atmo_def.h"
#include "atmosphere_model.hlsli"
#include "atmosphere_functions.hlsli"


struct VsOutput
{
	float4 pos_cs : SV_POSITION;
	noperspective float2 uv : TEXCOORD;
	nointerpolation uint slice_index : SLICE;
};

struct GsInput
{
	float4 pos_cs : SV_POSITION;
	float2 uv : TEXCOORD;
	uint slice_index : SLICE;
};

struct GsOutput
{
	float4 pos_cs : SV_POSITION;
	float2 uv : TEXCOORD;
	uint slice_index : SV_RENDERTARGETARRAYINDEX;
};


#pragma pack_matrix(row_major)
cbuffer precompute_cb : register(b0)
{
	float3x3 luminance_from_radiance : packoffset(c0);
	int scattering_order : packoffset(c4.x);
};


struct PsInputTransmittance {
	float4  pos_ss      : SV_POSITION;
	float2  uv          : TEXCOORD0;
};

struct PsInputDeltaIrradiance {
	float4  pos_ss      : SV_POSITION;
	float2  uv          : TEXCOORD0;
};

struct PsInputSingleScattering {
	float4  pos_ss      : SV_POSITION;
	float2  uv          : TEXCOORD0;
	uint    slice_index : SV_RENDERTARGETARRAYINDEX;
};

struct PsInputScatteringDensity {
	float4  pos_ss      : SV_POSITION;
	float2  uv          : TEXCOORD0;
	uint    slice_index : SV_RENDERTARGETARRAYINDEX;
};

struct PsInputIndirectIrradiance {
	float4  pos_ss      : SV_POSITION;
	float2  uv          : TEXCOORD0;
	uint    slice_index : SV_RENDERTARGETARRAYINDEX;
};

struct PsInputMultipleScattering {
	float4  pos_ss      : SV_POSITION;
	float2  uv          : TEXCOORD0;
	uint    slice_index : SV_RENDERTARGETARRAYINDEX;
};



struct PsOutputTransmittance
{
	float4 transmittance                : SV_TARGET0;
};

struct PsOutputDeltaIrradiance
{
	float3 delta_irradiance             : SV_TARGET0;
	float3 irradiance                   : SV_TARGET1;
};

struct PsOutputSingleScattering
{
	float3 delta_rayleigh               : SV_TARGET0;
	float3 delta_mie                    : SV_TARGET1;
	float4 scattering                   : SV_TARGET2;
	float3 single_mie                   : SV_TARGET3;
};

struct PsOutputScatteringDensity
{
	float3 scattering_density           : SV_TARGET0;
};

struct PsOutputIndirectIrradiance
{
	float3 delta_irradiance             : SV_TARGET0;
	float3 irradiance                   : SV_TARGET1;
};

struct PsOutputMultipleScattering
{
	float3 delta_multiple_scattering    : SV_TARGET0;
	float4 scattering                   : SV_TARGET1;
};

Texture2D transmittance_texture                 : register(t0);
Texture3D single_rayleigh_scattering_texture    : register(t1);
Texture3D single_mie_scattering_texture         : register(t2);
Texture3D multiple_scattering_texture           : register(t3);
Texture2D delta_irradiance_texture              : register(t4);
Texture3D scattering_density_texture            : register(t1);

SamplerState atmosphere_sampler : register(s0)
//SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};


VsOutput VS(uint vertex_id : SV_VERTEXID, uint inst_id : SV_INSTANCEID)
{
	float2 uv = float2((vertex_id << 1) & 2, vertex_id & 2);
	float4 pos_cs = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	VsOutput result = (VsOutput)0;
	result.uv = float2(uv.x, uv.y);
	result.pos_cs = pos_cs;
	result.slice_index = inst_id;

	return result;
}


[maxvertexcount(3)]
void GS(triangle GsInput input[3], inout TriangleStream<GsOutput> output)
{
	[unroll]
	for (uint i = 0; i < 3; i++) {
		GsOutput tri;
		tri.pos_cs = input[i].pos_cs;
		tri.uv = input[i].uv;
		tri.slice_index = input[i].slice_index;
		output.Append(tri);
	}
}


PsOutputTransmittance PS_Transmittance(PsInputTransmittance input)
{
	PsOutputTransmittance output = (PsOutputTransmittance)0;
	float2 coord_ss = input.pos_ss.xy;
	output.transmittance = float4(ComputeTransmittanceToTopAtmosphereBoundaryTexture(atmosphere, coord_ss), 1.0);
	return output;
}

PsOutputDeltaIrradiance PS_DeltaIrradinace(PsInputDeltaIrradiance input)
{
	PsOutputDeltaIrradiance output = (PsOutputDeltaIrradiance)0;
	float2 coord_ss = input.pos_ss.xy;
	output.delta_irradiance = ComputeDirectIrradianceTexture(atmosphere, transmittance_texture, atmosphere_sampler, coord_ss);
	output.irradiance = (float3) 0.0;
	return output;
}

PsOutputSingleScattering PS_SingleScattering(PsInputSingleScattering input)
{
	PsOutputSingleScattering output = (PsOutputSingleScattering)0;
	float2 coord_ss = input.pos_ss.xy;
	ComputeSingleScatteringTexture(atmosphere, transmittance_texture, atmosphere_sampler
		, float3(coord_ss, input.slice_index + 0.5), output.delta_rayleigh, output.delta_mie);
	output.scattering = float4(mul(luminance_from_radiance, output.delta_rayleigh)
		, mul(luminance_from_radiance, output.delta_mie).r);
	output.single_mie = mul(luminance_from_radiance, output.delta_mie);
	return output;
}

PsOutputScatteringDensity PS_ScatteringDensity(PsInputScatteringDensity input)
{
	PsOutputScatteringDensity output = (PsOutputScatteringDensity)0;
	float2 coord_ss = input.pos_ss.xy;
	output.scattering_density = ComputeScatteringDensityTexture(atmosphere, transmittance_texture, single_rayleigh_scattering_texture, single_mie_scattering_texture,
		multiple_scattering_texture, delta_irradiance_texture, atmosphere_sampler, float3(coord_ss, input.slice_index + 0.5), scattering_order);
	return output;
}

PsOutputIndirectIrradiance PS_IndirectIrradiance(PsInputIndirectIrradiance input)
{
	PsOutputIndirectIrradiance output = (PsOutputIndirectIrradiance)0;
	float2 coord_ss = input.pos_ss.xy;
	output.delta_irradiance = ComputeIndirectIrradianceTexture(atmosphere
		, single_rayleigh_scattering_texture, single_mie_scattering_texture
		, multiple_scattering_texture, atmosphere_sampler
		, float3(coord_ss, input.slice_index + 0.5), scattering_order);
	output.irradiance = mul(luminance_from_radiance, output.delta_irradiance);
	return output;
}

PsOutputMultipleScattering PS_MultipleScattering(PsInputMultipleScattering input)
{
	PsOutputMultipleScattering output = (PsOutputMultipleScattering)0;
	float2 coord_ss = input.pos_ss.xy;
	float nu = 0.0;
	output.delta_multiple_scattering = ComputeMultipleScatteringTexture(atmosphere, transmittance_texture
		, scattering_density_texture, atmosphere_sampler, float3(coord_ss, input.slice_index + 0.5), nu);
	output.scattering = float4(mul(luminance_from_radiance, output.delta_multiple_scattering / RayleighPhaseFunction(nu)), 0.0);
	return output;
}



technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Transmittance()));
	}
}

technique11 TechTransmittance
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Transmittance()));
	}
}

technique11 TechDeltaIrradiance
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DeltaIrradinace()));
	}
}

technique11 TechSingleScattering
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		//SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_SingleScattering()));
	}
}

technique11 TechScatteringDensity
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
//		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ScatteringDensity()));
	}
}

technique11 TechIndirectIrradiance
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
//		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_IndirectIrradiance()));
	}
}

technique11 TechMultipleScattering
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
//		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_MultipleScattering()));
	}
}
