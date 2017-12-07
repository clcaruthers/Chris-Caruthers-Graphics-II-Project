#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 UV : TEXCOORD;
	float4 norm : NORMAL;
	float4 color : COLOR;
};

struct OUTPUT_VERTEX
{
	float4 colorOut : COLOR;
	float4 projectedCoordinate : SV_POSITION;
	float4 UVout : TEXCOORD;
	float4 normOut : NORMAL;
	float4 worldPos : WORLDPOS;
	float4 localPos : LOCALPOS;
};

// TODO: PART 3 STEP 2a
cbuffer THIS_IS_VRAM : register(b0)
{
	float4x4 worldMat;
	float4x4 viewMat;
	float4x4 projMat;
};

cbuffer waveInfo : register(b1) {
	float offset;
}

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	sendToRasterizer.localPos = fromVertexBuffer.coordinate;
	sendToRasterizer.projectedCoordinate = fromVertexBuffer.coordinate;

	sendToRasterizer.projectedCoordinate.x = sendToRasterizer.projectedCoordinate.x + (sin(sendToRasterizer.projectedCoordinate.z + offset) / 8.0f);

	sendToRasterizer.UVout = fromVertexBuffer.UV;
	sendToRasterizer.normOut = fromVertexBuffer.norm;
	sendToRasterizer.colorOut = fromVertexBuffer.color;

	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, worldMat);
	sendToRasterizer.worldPos = sendToRasterizer.projectedCoordinate;
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, viewMat);
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, projMat);

	return sendToRasterizer;
}