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

[maxvertexcount(9)]

void main(point GSOutput input[1], inout TriangleStream< GSOutput > output)
{
	float of = 0;
	for (uint i = 0; i < 9; i++)
	{
		if (i == 3 || i == 6) {
			of += 0.7f;
			output.RestartStrip();
		}

		GSOutput element;
		element.col = input[0].col;
		element.UVS = input[0].UVS;
		element.n = input[0].n;
		element.worldp = input[0].worldp;
		element.localp = input[0].localp;

		if (i == 0 || i == 3 || i == 6) {
			element.pos = input[0].pos;
			element.pos.x += 0.3f + of;
			element.pos.z += of;
		}
		else if (i == 1 || i == 4 || i == 7) {
			element.pos = input[0].pos;
			element.pos.x -= 0.3f;
			element.pos.x += of;
			element.pos.z += of;
		}
		else {
			element.pos = input[0].pos;
			element.pos.x += of;
			element.pos.y += 1;
			element.pos.z += of;
		}

		element.pos = mul(element.pos, viewMat);
		element.pos = mul(element.pos, projMat);
		output.Append(element);
	}
}