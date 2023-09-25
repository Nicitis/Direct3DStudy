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
    
    
    // ������ ��� ������ ���� ��ġ ���͸� 4�������� ��ȯ�Ѵ�.
    input.position.w = 1.0f;
    
    // ����, ��, ���� ��Ŀ� ���� ������ ��ġ�� ����Ѵ�.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // �ȼ� ���̴��� ���� �ؽ�ó ��ǥ�� �����Ѵ�.
    
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
    
    // �ؽ�ó ��ǥ ��ġ���� ���÷��� ����ؼ� �ؽ�ó�κ��� �ȼ� ������ ���ø��Ѵ�.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    return textureColor;
}