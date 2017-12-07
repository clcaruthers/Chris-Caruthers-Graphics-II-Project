#pragma pack_matrix(row_major)

struct GSOutput
{
	float4 col : COLOR;
	float4 pos : SV_POSITION;
	float4 UVS : TEXCOORD;
	float4 n : NORMAL;
	float4 worldp : WORLDPOS;
	float4 localp : LOCALPOS;
};

cbuffer THIS_IS_VRAM : register(b0)
{
	float4x4 worldMat;
	float4x4 viewMat;
	float4x4 projMat;
};

[maxvertexcount(3)]

void main(point GSOutput input[1], inout TriangleStream< GSOutput > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.col = input[0].col;
		element.UVS = input[0].UVS;
		element.n = input[0].n;
		element.worldp = input[0].worldp;
		element.localp = input[0].localp;

		if (i == 0) {
			element.pos = input[0].pos;
			element.pos.x += 0.3f;
		}
		else if (i == 1) {
			element.pos = input[0].pos;
			element.pos.x -= 0.3f;
		}
		else {
			element.pos = input[0].pos;
			element.pos.y += 1;
		}
		element.pos = mul(element.pos, viewMat);
		element.pos = mul(element.pos, projMat);
		output.Append(element);
	}
}