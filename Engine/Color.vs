cbuffer Matrix
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType InInput)
{
	PixelInputType output;

	InInput.position.w = 1.0f;

	output.position = mul(InInput.position, WorldMatrix);
	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, WorldMatrix);

	output.color = InInput.color;

	return output;
}
