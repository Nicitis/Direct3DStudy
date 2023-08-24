////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
//#include <cstdio>


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Direct3D ��ü�� �����ϰ� �ʱ�ȭ�Ѵ�.
	m_Direct3D = std::make_unique<D3DClass>();

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not intialize Direct3D", L"Error", MB_OK);
		return false;
	}
	
	// ī�޶� ��ü�� �����Ѵ�.
	m_Camera = std::make_unique<CameraClass>();

	// ī�޶��� �ʱ� ��ġ�� �����Ѵ�.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// �� ��ü�� ���� �� �ʱ�ȭ�Ѵ�.
	m_Model = std::make_unique<ModelClass>();

	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// �÷� ���̴� ��ü�� ���� �� �ʱ�ȭ�Ѵ�.
	m_ColorShader = std::make_unique<ColorShaderClass>();

	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// ����: �ؽ�Ʈ ���Ͽ� �׷��� ī�� ����
	//WriteVideoCardToFile("Video Card Info.txt");

	return true;
}


void ApplicationClass::Shutdown()
{
	// �÷� ���̴� ��ü�� �����Ѵ�.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		m_ColorShader = 0;
	}

	// �� ��ü�� �����Ѵ�.
	if (m_Model)
	{
		m_Model->Shutdown();
		m_Model = 0;
	}

	// ī�޶� ��ü�� �����Ѵ�.
	if (m_Camera)
	{
		m_Camera = 0;
	}

	// Direct3D ��ü�� �Ҵ� �����Ѵ�.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;

	// �׷��Ƚ� ���� �������Ѵ�.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// ���۸� ��� ���� �����Ѵ�.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ���� ī�޶��� ��ġ�� ���� �� ����� �����Ѵ�.
	m_Camera->Render();

	// ����, ��, ���� ����� ī�޶�� d3d ��ü�κ��� ȹ���Ѵ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// �� ���� �� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� �׸� �غ� �Ѵ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// �÷� ���̴��� ����� ���� �������Ѵ�.
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// ���� �����ϱ� ���� ���۸� �ʱ�ȭ�Ѵ�.
	// m_Direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);

	// �������� ���� ȭ�� �� �����Ѵ�.
	m_Direct3D->EndScene();

	return true;
}


// Ʃ�丮�� 3 ���� 4: �׷��� ī���� �̸��� �޸𸮸� ���Ͽ� ����Ѵ�.
//bool ApplicationClass::WriteVideoCardToFile(const char* fileName)
//{
//	FILE* fp;
//	errno_t result = fopen_s(&fp, fileName, "w");
//	if (result != 0)
//	{
//		return false;
//	}
//	char videoCardName[128];
//	int memory;
//	m_Direct3D->GetVideoCardInfo(videoCardName, memory);
//	
//	fprintf(fp, "Video card information\nvideo card name: %s\ndedicated card memory: %d MB",
//		videoCardName, memory);
//	
//	return true;
//}