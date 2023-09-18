////////////////////////////////////////////////////////////////////////////////
// Filename: color.hlsl
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer EveryFrameChangedBuffer : register(b1)
{
    float time;
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

///////////////////////////////////
// Pixel Shader
///////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
    //return input.color;
    int i = floor(time);
    float f = frac(time);
    float mirroredTime;
    if (i % 2 == 0)
        mirroredTime = f;
    else
        mirroredTime = 1 - f;
    
    float smoothTime = smoothstep(0, 1, mirroredTime);
    
    return float4(input.color.xyz * smoothTime, 1);
}