texture2D tex : register(t0);

SamplerState SS : register(s0);

cbuffer DIRLIGHT : register(b0) {
	float4 dir;
	float4 rgb;
}

float4 main( float4 colorFromRasterizer : COLOR, float4 XYZW : SV_POSITION, float4 UV : TEXCOORD, float4 norm : NORMAL ) : SV_TARGET
{
	float4 dirNorm = normalize(dir);
	float4 texColor = tex.Sample(SS, UV.xy);
	float rat = saturate(dot(dir, norm)) + 0.25f;
	
	return rat * rgb * texColor;
}