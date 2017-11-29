texture2D tex : register(t0);

SamplerState SS : register(s0);

float4 main( float4 colorFromRasterizer : COLOR, float4 XYZW : SV_POSITION, float4 UV : TEXCOORD, float4 norm : NORMAL ) : SV_TARGET
{
	return tex.Sample(SS, UV.xy);
}