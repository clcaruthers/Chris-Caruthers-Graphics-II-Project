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

cbuffer CONELIGHT : register(b2) {
	float4 clpos;
	float4 clcol;
	float4 cldir;
	float coneratio;
	float3 pad2;
}

cbuffer SPECULAR : register(b3) {
	float4 camPos;
	float4 specPow;
}

float4 main(float4 colorFromRasterizer : COLOR, float4 XYZW : SV_POSITION, float4 UV : TEXCOORD, float4 norm : NORMAL, float4 WP : WORLDPOS, float4 LP : LOCALPOS) : SV_TARGET
{
	int shiny = 16384;
	float4 dirNorm = normalize(dir);
	float4 texColor = tex.Sample(SS, UV.xy);
	if (texColor.x > (texColor.y + texColor.z) + 0.1f) {
		texColor.z = texColor.x;
		texColor.z = texColor.z / 1.2f;
		texColor.x = texColor.x / 1.2f;
	}
	/*float temp = texColor.x;
	texColor.x = texColor.z;
	texColor.z = texColor.y;
	texColor.y = temp;*/
	float rat = saturate(saturate(dot(-dirNorm, norm)) + 0.2f);

	float3 camAngle = normalize(camPos - WP);
	float3 ref = normalize(reflect(dirNorm, norm));
	float specInt = saturate(dot(ref, camAngle));
	float spec = pow(specInt, specPow) * shiny;

	float3 res1 = rgb * (rat + spec);//lerp(float4(0, 0, 0, 1), rgb, saturate(rat + spec));

	float4 pdir = normalize(loc - WP);
	float d = dot(pdir, norm);
	float prat = saturate(d);
	float atn = 1.0f - saturate(length(loc - WP) / rad);
	atn = atn * atn;
	float4 res2 = lerp(float4(0, 0, 0, 1), col, prat) * atn;

	float4 conedir = normalize(cldir);
	float4 cdir = normalize(clpos - WP);
	float surfaceratio = saturate(dot(-cdir, conedir));
	float spotfactor = (surfaceratio > coneratio) ? 1 : 0;
	float lightratio = saturate(dot(cdir, norm));
	float3 cref = normalize(reflect(-cdir, norm));
	float cspecInt = saturate(dot(cref, camAngle));
	float cspec = pow(cspecInt, specPow) * shiny;
	float4 res3 = spotfactor * (lightratio + cspec) * clcol;

	float4 newcol;
	newcol.x = texColor.x;
	newcol.y = texColor.y;
	newcol.z = texColor.z;
	newcol.w = 0.65f;

	float3 o = saturate(saturate(res1 + res2 + res3) * newcol.xyz);
	float4 o2;
	o2.xyz = o;
	o2.w = newcol.w;

	return o2;
}