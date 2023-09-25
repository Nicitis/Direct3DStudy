////////////////////////////////////////////////////////////////////////////////
// Filename: texture.hlsl
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

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


///////////////////////////////////
// Vertex Shader
///////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
    // 적절한 행렬 연산을 위해 위치 벡터를 4차원으로 변환한다.
    input.position.w = 1.0f;
    
    // 월드, 뷰, 투영 행렬에 대해 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이더를 위해 텍스처 좌표를 저장한다.
    
    output.tex = input.tex;
       
    return output;
}

///////////////////////////////////
// Pixel Shader
///////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
    
    //return 1.0f.xxxx;
    
    // 텍스처 좌표 위치에서 샘플러를 사용해서 텍스처로부터 픽셀 색상을 샘플링한다.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    return textureColor;
}