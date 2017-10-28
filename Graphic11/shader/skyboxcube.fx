
//--------------------------------------------------------------------------------------
// Texture
//--------------------------------------------------------------------------------------
Texture2D txDiffuse	: register(t0);
Texture2D txBump	: register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txSkybox : register(t4);

float3 g_AtmosphereBrightColor = { 1.0,1.1,1.4 };
float3 g_AtmosphereDarkColor = { 0.6,0.6,0.7 };

SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register(b0)
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
	matrix gLightView[3];
	matrix gLightProj[3];
	matrix gLightTT;
	float3 gEyePosW;
}

cbuffer cbLight : register(b1)
{
	float4 gLight_Ambient;
	float4 gLight_Diffuse;
	float4 gLight_Specular;
	float3 gLight_Direction;
	float3 gLight_PosW;
}

cbuffer cbClipPlane : register(b4)
{
	float4	gClipPlane;
}


//--------------------------------------------------------------------------------------
struct VSIn_Default
{
	float4 position : POSITION;
	float2 texcoord  : TEXCOORD;
};

struct PSIn_Diffuse
{
	float4 position     : SV_Position;
	centroid float2 texcoord     : TEXCOORD0;
	centroid float3 positionWS   : TEXCOORD1;
	float clip : SV_ClipDistance0;
};


struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD1;
};

// primitive simulation of non-uniform atmospheric fog
float3 CalculateFogColor(float3 pixel_to_light_vector, float3 pixel_to_eye_vector)
{
	return lerp(g_AtmosphereDarkColor, g_AtmosphereBrightColor, 0.5*dot(pixel_to_light_vector, -pixel_to_eye_vector) + 0.5);
}


PSIn_Diffuse VS(VSIn_Default input)
{
	PSIn_Diffuse output;
	output.position = mul(input.position, gWorld);
	output.position = mul(output.position, gView);
	output.position = mul(output.position, gProjection);
	output.positionWS = input.position.xyz;
	output.texcoord = input.texcoord;

	output.clip = dot(mul(input.position, gWorld), gClipPlane);
	return output;
}


float4 PS(PSIn_Diffuse input) : SV_Target
{
	float4 color;
	float3 acolor;
	float3 pixel_to_light_vector = normalize(gLight_PosW - input.positionWS);
	float3 pixel_to_eye_vector = normalize(gEyePosW - input.positionWS);

	color = txSkybox.Sample(samLinear,float2(input.texcoord.x,pow(input.texcoord.y,2)));
	acolor = CalculateFogColor(pixel_to_light_vector,pixel_to_eye_vector);
	color.rgb = lerp(color.rgb,acolor,pow(saturate(input.texcoord.y),10));
	color.a = 1;
	//return float4(0, 0, 0, 1);
	return color;
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

