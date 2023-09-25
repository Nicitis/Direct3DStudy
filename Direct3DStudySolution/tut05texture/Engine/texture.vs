////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
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