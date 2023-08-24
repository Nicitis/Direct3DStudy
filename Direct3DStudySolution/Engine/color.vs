////////////////////////////////////////////////////////////////////////////////
// Filename: color.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
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

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	
	// �ùٸ� ��� ������ ���� position ���͸� 4�� ������ �ִ� ������ �ٲ۴�.
	input.position.w = 1.0f;

	// ����, ��, ���� ����� ��������(against) ������ ��ġ�� ����Ѵ�.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �Է� ������ �ȼ� ���̴��� ����� �� �ְ� �����Ѵ�.
	output.color = input.color;

	return output;
}