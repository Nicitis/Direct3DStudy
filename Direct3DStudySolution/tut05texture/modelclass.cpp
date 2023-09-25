////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool result;

	
	// ���� �� �ε��� ���۸� �ʱ�ȭ�Ѵ�.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// �ؽ�ó�� �ҷ��´�
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		MessageBox(NULL, L"cannot load texture", L"error", MB_OK);
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// �� �ؽ�ó�� �����Ѵ�.
	ReleaseTexture();

	// ���� �� �ε��� ���۸� �����Ѵ�.
	ShutdownBuffers();

	return;
}


void ModelClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����Ѵ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����Ѵ�.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �׸��� ������ ���� ���� �� �ε��� ���۸� �׷��Ƚ� ���������ο� �ִ´�.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ���� �迭���� ������ ���� �����Ѵ�.
	m_vertexCount = 3;

	// �ε��� �迭���� �ε����� ���� �����Ѵ�.
	m_indexCount = 3;

	// ���� �迭�� �����Ѵ�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� �����Ѵ�.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ���� �迭�� ���� �ִ´�.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // ���� �Ʒ�
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[0].color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // ���� ���
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	//vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // ������ �Ʒ�
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// �ε��� �迭�� ���� �ִ´�.
	indices[0] = 0; // ���� �Ʒ�
	indices[1] = 1; // ���� ��
	indices[2] = 2; // ������ �Ʒ�

	// CreateBox(0.0f, 0.0f, 0.0f, 3.0f, &vertices, &indices);


	// ���� ���� ������ description�� �����Ѵ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ����ü�� ���� �����Ϳ� ���� �����͸� �����Ѵ�.
	vertexData.pSysMem = vertices;	// �����͸� ���� ���� �迭�� �ʱ�ȭ
	vertexData.SysMemPitch = 0;		// �ؽ�ó(2D, 3D ����)���� ���� ���� ���۵� �������� ����
	vertexData.SysMemSlicePitch = 0;// 3D �ؽ�ó���� ���� ���� ������ ���۵� �������� ����

	// ���� ���� �迭�� �����Ѵ�
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ����(static) �ε��� ���ۿ� ���� description�� �ۼ��Ѵ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ����ü�� �ε��� �����Ϳ� ���� �����͸� �����Ѵ�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����Ѵ�
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �� �ε��� ���۸� ����� ���� �Ҵ������Ƿ� �迭���� �������ش�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// ���� ������ ����(stride)�� �������� �����Ѵ�.
	stride = sizeof(VertexType);
	offset = 0;

	// �Է� ������� ���� ���۸� Ȱ��ȭ�Ͽ� �������� �� �ְ� �Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �Է� ������� �ε��� ���۸� Ȱ��ȭ�Ͽ� �������� �� �ְ� �Ѵ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻��(primitive)�� �����Ѵ�. �� ���� �ﰢ���� �ǰڴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// �ؽ�ó ��ü�� ���� �� �ʱ�ȭ�Ѵ�.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// �ؽ�ó ��ü�� �����Ѵ�.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


/*
bool ModelClass::CreateBox(float centerX, float centerY, float centerZ, float size, VertexType** vertices, unsigned long** indices)
{
	// ���� �迭���� ������ ���� �����Ѵ�.
	m_vertexCount = 8;

	// �ε��� �迭���� �ε����� ���� �����Ѵ�.
	m_indexCount = 36;

	// ���� �迭�� �����Ѵ�.
	*vertices = new VertexType[m_vertexCount];
	if (!(*vertices))
	{
		return false;
	}

	// �ε��� �迭�� �����Ѵ�.
	*indices = new unsigned long[m_indexCount];
	if (!(*indices))
	{
		return false;
	}

	// ���� �迭�� ���� �ִ´�.
	int index = 0;
	VertexType* verticesArray = *vertices;
	float basePointX = centerX - size * 0.5f;
	float basePointY = centerY - size * 0.5f;
	float basePointZ = centerZ - size * 0.5f;

	for (int y = 0; y < 2; y++)
	{
		for (int z = 0; z < 2; z++)
		{
			for (int x = 0; x < 2; x++)
			{
				verticesArray[index].position = XMFLOAT3(basePointX + size * x, basePointY + size * y, basePointZ + size * z);
				verticesArray[index].color = XMFLOAT4(x, y, z, 1.0f);
				index++;
			}
		}
	}
	
	// �ε��� �迭�� ���� �ִ´�.
	// y
	// |  z
	// | /
	// |/
	// |--- x

	//    6 ----- 7
	//   /       /|
	//  /       / |
	// 4 ----- 5  |
 	// |  2 ---|- 3
	// | /     | /
	// |/      |/
	// 0 ----- 1
	
	unsigned long tempIndices[36] =
	{
		// Bottom
		0, 1, 2,
		2, 1, 3,
		// Front
		0, 4, 5,
		0, 5, 1,
		// Right
		1, 5, 7,
		1, 7, 3,
		// Back
		2, 6, 4,
		2, 4, 0,
		// Left
		3, 7, 6,
		3, 6, 2,
		// Top
		4, 6, 7,
		4, 7, 5
	};

	for (int i = 0; i < m_indexCount; i++)
	{
		(*indices)[i] = tempIndices[i];
	}

	return true;
}
*/