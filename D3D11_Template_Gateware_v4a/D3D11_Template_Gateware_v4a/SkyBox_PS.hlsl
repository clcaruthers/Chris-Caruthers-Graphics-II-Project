textureCUBE tex : register(t0);

SamplerState SS : register(s0);

float4 main(float4 colorFromRasterizer : COLOR, float4 XYZW : SV_POSITION, float4 UV : TEXCOORD, float4 norm : NORMAL, float4 WP : WORLDPOS, float4 LP : LOCALPOS) : SV_TARGET
{
	return tex.Sample(SS, LP.xyz);
}