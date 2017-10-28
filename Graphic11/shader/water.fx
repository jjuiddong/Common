//=============================================================================
// Water.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Scrolls normal maps over for per pixel lighting of high 
// frequency waves; uses an environment map for reflections.
//
// 2017-09-11
//	- Upgrade DX11
//=============================================================================

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txBump	: register(t1);
Texture2D txWaveMap0 : register(t4);
Texture2D txWaveMap1 : register(t5);
Texture2D txReflectMap : register(t6);
Texture2D txRefractMap : register(t7);

SamplerState samLinear : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samClamp : register(s1)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


RasterizerState Depth
{
	DepthBias = 10000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};


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


cbuffer cbMaterial : register(b2)
{
	float4 gMtrl_Ambient;
	float4 gMtrl_Diffuse;
	float4 gMtrl_Specular;
	float4 gMtrl_Emissive;
	float gMtrl_Pow;
}


cbuffer cbWater : register(b3)
{
	matrix	gWorldInv;
	float2  gWaveMapOffset0;
	float2  gWaveMapOffset1;
	float2  gRippleScale;
	float   gRefractBias;
	float   gRefractPower;
}


cbuffer cbClipPlane : register(b4)
{
	float4	gClipPlane;
}


float4 g_vFog= {1, 7000, 0, 0};
float4 g_fogColor = {96.f/255.f, 76.f/255.f, 64.f/255.f, 1}; // RGB(96, 76, 64)


struct OutputVS
{
    float4 posH			: SV_POSITION;
    float3 toEyeT		: TEXCOORD0;
    float3 lightDirT	: TEXCOORD1;
    float2 tex0			: TEXCOORD2;
    float2 tex1			: TEXCOORD3;
    float4 projTexC		: TEXCOORD4;
    float  eyeVertDist	: TEXCOORD5;
    float3 Eye			: TEXCOORD6;
};


OutputVS WaterVS(
	float4 posL : POSITION
	, float3 Normal : NORMAL
	, float2 tex0 : TEXCOORD0
	)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Build TBN-basis.  For flat water grid in the xz-plane in 
	// object space, the TBN-basis has a very simple form.
	float3x3 TBN;
	TBN[0] = float3(1.0f, 0.0f, 0.0f); // Tangent goes along object space x-axis.
	TBN[1] = float3(0.0f, 0.0f, -1.0f);// Binormal goes along object space -z-axis
	TBN[2] = float3(0.0f, 1.0f, 0.0f); // Normal goes along object space y-axis

	//matrix mWVP = mul(gWorld, gView);
	//mWVP = mul(mWVP, gProjection);
	
	// Matrix transforms from object space to tangent space.
	float3x3 toTangentSpace = transpose(TBN);
	
	// Transform eye position to local space.
	float3 eyePosL = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;
	outVS.eyeVertDist = distance(float4(eyePosL,1), posL);
	
	// Transform to-eye vector to tangent space.
	float3 toEyeL = eyePosL - posL.xyz;
	outVS.toEyeT = mul(toEyeL, toTangentSpace);
	
	// Transform light direction to tangent space.
	float3 lightDirL = mul(float4(gLight_Direction, 0.0f), gWorldInv).xyz;
	outVS.lightDirT  = mul(lightDirL, toTangentSpace);
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(posL, gWorld);
	outVS.posH = mul(outVS.posH, gView);
	outVS.posH = mul(outVS.posH, gProjection);
	//outVS.posH = mul(posL, mWVP);
	
	// Scroll texture coordinates.
	outVS.tex0 = tex0 + gWaveMapOffset0;
	outVS.tex1 = tex0 + gWaveMapOffset1;
	
	// Generate projective texture coordinates from camera's perspective.
	outVS.projTexC = outVS.posH;

	outVS.Eye = gEyePosW - mul(posL, gWorld).xyz;

	// Done--return the output.
	return outVS;
}


float4 WaterPS( OutputVS In ) : SV_Target
{
	// Interpolated normals can become unnormal--so normalize.
	// Note that toEyeW and normalW do not need to be normalized
	// because they are just used for a reflection and environment
	// map look-up and only direction matters.
	In.toEyeT    = normalize(In.toEyeT);
	In.lightDirT = normalize(In.lightDirT);
	
	// Light vector is opposite the direction of the light.
	float3 lightVecT = -In.lightDirT;
	
	// Sample normal map.
	//float3 normalT0 = tex2D(WaveMapS0, tex0);
	float3 normalT0 = txWaveMap0.Sample(samLinear, In.tex0).xyz;
	//float3 normalT1 = tex2D(WaveMapS1, tex1);
	float3 normalT1 = txWaveMap1.Sample(samLinear, In.tex1).xyz;
	
	// Expand from [0, 1] compressed interval to true [-1, 1] interval.
    normalT0 = 2.0f*normalT0 - 1.0f;
    normalT1 = 2.0f*normalT1 - 1.0f;
    
	// Average the two vectors.
	float3 normalT = normalize(0.5f*(normalT0 + normalT1));
	
	// Compute the reflection vector.
	float3 r = reflect(-lightVecT, normalT);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, In.toEyeT), 0.0f), gMtrl_Pow);
	
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVecT, normalT), 0.0f);
	
	// If the diffuse light intensity is low, kill the specular lighting term.
	// It doesn't look right to add specular light when the surface receives 
	// little diffuse light.
	if(s <= 0.0f)
	     t = 0.0f;
	     
	// Project the texture coordinates and scale/offset to [0,1].
	In.projTexC.xy /= In.projTexC.w;
	//In.projTexC.x =  0.5f*In.projTexC.x + 0.5f;
	In.projTexC.x = 0.5f*In.projTexC.x + 0.53f;
	In.projTexC.y = -0.5f*In.projTexC.y + 0.47f;

	// To avoid clamping artifacts near the bottom screen edge, we 
	// scale the perturbation magnitude of the v-coordinate so that
	// when v is near the bottom edge of the texture (i.e., v near 1.0),
	// it doesn't cause much distortion.  The following power function
	// scales v very little until it gets near 1.0.
	// (Plot this function to see how it looks.)
	float vPerturbMod = -pow(abs(In.projTexC.y), 10.0f) + 1.0f;
	
	// Sample reflect/refract maps and perturb texture coordinates.
	float2 perturbVec = normalT.xz*gRippleScale;
	perturbVec.y *= vPerturbMod;
	//float3 reflectCol = tex2D(ReflectMapS, projTexC.xy+perturbVec).rgb;
	float3 reflectCol = txReflectMap.Sample(samClamp, In.projTexC.xy + perturbVec).rgb;
	//float3 refractCol = tex2D(RefractMapS, projTexC.xy+perturbVec).rgb;
	float3 refractCol = txRefractMap.Sample(samClamp, In.projTexC.xy + perturbVec).rgb;

	// Refract based on view angle.
	float refractWt = saturate(gRefractBias+pow(max(dot(In.toEyeT, normalT), 0.0f), gRefractPower));
	
	// Weighted average between the reflected color and refracted color, modulated
	// with the material.
	float3 ambientMtrl = gMtrl_Ambient.xyz*lerp(reflectCol, refractCol, refractWt).xyz;
	float3 diffuseMtrl = gMtrl_Diffuse.xyz*lerp(reflectCol, refractCol, refractWt).xyz;
	
	// Compute the ambient, diffuse and specular terms separatly. 
	float3 spec = t*(gMtrl_Specular*gLight_Specular).rgb;
	float3 diffuse = (diffuseMtrl*gLight_Diffuse.rgb);
	float3 ambient = ambientMtrl.xyz*gLight_Ambient.xyz;
	
	float3 final = diffuse + spec;
	
	// Output the color and the alpha.
   	//return float4(final, gMtrl_Diffuse.a);

	float4 color = float4(final, gMtrl_Diffuse.a);
	float distance = length(In.Eye);
	float l = saturate((distance - g_vFog.x) / (g_vFog.y - g_vFog.x));
	float3 Out = lerp(color.xyz, g_fogColor.xyz, l);
	return float4(Out.xyz, color.w);
	
	//return float4(reflectCol, 1);
	//return float4(txDiffuse.Sample(samLinear, In.tex0).rgb, 1);
	//return float4(refractCol,1);
	//return float4(reflectCol,1);
	//return float4(final, 1);
	//return txReflectMap.Sample(samClamp, In.tex0);
	//return float4(1,1,1,1);	
	//return float4(final, 1);
}


technique11 Unlit
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_5_0, WaterVS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, WaterPS()));
    }    
}
