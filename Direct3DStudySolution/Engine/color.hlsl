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

	
	// 올바른 행렬 연산을 위해 position 벡터를 4개 성분이 있는 것으로 바꾼다.
    input.position.w = 1.0f;

	// 월드, 뷰, 투영 행렬을 기준으로(against) 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// 입력 색상을 픽셀 셰이더가 사용할 수 있게 저장한다.
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