void GodRays_VS(
    float4 position			: POSITION,
	float2 uv               : TEXCOORD0,
	out float4 oPos		    : POSITION,
	out float2 oTexCoord	: TEXCOORD0,
	out float2 oLightPos    : TEXCOORD1,
	uniform float4 lightPosition
	 
) {
    oPos = float4(position.xy, 0, 1);
	oTexCoord = uv;
	
	oLightPos = lightPosition.xy;
}

void GodRays_FP(
    float2 uv			: TEXCOORD0,
	float2 lightPosition : TEXCOORD1,
	uniform sampler2D frame : register(s0),
    out float4 color		: COLOR,
    uniform float den
   
) {
	const float density = 0.5;
	const int samples = 16;
	const float weight = 0.7;
	const float decay = 1.06;
	
	float2 deltaTexCoord = (uv - lightPosition);
	
	deltaTexCoord *= 1.0f / samples*0.2;
	
	float3 col = tex2D(frame, uv).rgb;
	float illuminationDecay = 1.0f;
	if (den!=0)
     {
	for (int i = 0; i < samples; i++) {
		uv -= deltaTexCoord;
		float3 sample = tex2D(frame, uv).rgb;
		sample *= illuminationDecay * den;
		col += sample;
		illuminationDecay *= decay;
	}
	}

	color = float4(col * 0.03, 1);
}