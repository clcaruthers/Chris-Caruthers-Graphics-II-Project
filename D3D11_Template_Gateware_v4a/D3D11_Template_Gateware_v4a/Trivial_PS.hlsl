texture2D tex : register(t0);

SamplerState SS : register(s0);

cbuffer DIRLIGHT : register(b0) {
	float4 dir;
	float4 rgb;
}

cbuffer PLIGHT : register(b1) {
	float4 loc;
	float4 col;
	float rad;
	float3 pad;
}

float4 main(float4 colorFromRasterizer : COLOR, float4 XYZW : SV_POSITION, float4 UV : TEXCOORD, float4 norm : NORMAL, float4 WP : WORLDPOS) : SV_TARGET
{
	float4 dirNorm = normalize(dir);
	float4 texColor = tex.Sample(SS, UV.xy);
	float rat = saturate(saturate(dot(dirNorm, norm)) + 0.25f);
	float4 res1 = lerp(float4(0, 0, 0, 0), rgb, rat);

	float4 pdir = normalize(loc - WP);
	float d = dot(pdir, norm);
	float prat = saturate(d);
	float atn = 1.0f - saturate(length(loc - WP) / rad);
	atn = atn * atn;
	float4 res2 = lerp(float4(0, 0, 0, 0), col, prat) * atn;

	return saturate(saturate(res1 + res2) * texColor);
}