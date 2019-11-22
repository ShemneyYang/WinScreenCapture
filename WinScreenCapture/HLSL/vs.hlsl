struct VSInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};
struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
//vertex shader
cbuffer MatrixBuffer
{
	matrix mvp;
};
PSInput VS(VSInput input)
{
	PSInput output;
	input.position.w = 1.0f;
	output.position = mul(input.position, mvp);
	output.tex = input.tex;
	return output;
}