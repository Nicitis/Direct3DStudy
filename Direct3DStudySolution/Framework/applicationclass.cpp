////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
//#include <iostream>
#include <cstdio>


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
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

	// ����: �ؽ�Ʈ ���Ͽ� �׷��� ī�� ����
	WriteVideoCardToFile("Video Card Info.txt");

	return true;
}


void ApplicationClass::Shutdown()
{
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
	// ���� �����ϱ� ���� ���۸� �ʱ�ȭ�Ѵ�.
	m_Direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);

	// �������� ���� ȭ�� �� �����Ѵ�.
	m_Direct3D->EndScene();

	return true;
}


// Ʃ�丮�� 3 ���� 4: �׷��� ī���� �̸��� �޸𸮸� ���Ͽ� ����Ѵ�.
bool ApplicationClass::WriteVideoCardToFile(const char* fileName)
{
	FILE* fp;
	errno_t result = fopen_s(&fp, fileName, "w");
	if (result != 0)
	{
		return false;
	}
	char videoCardName[128];
	int memory;
	m_Direct3D->GetVideoCardInfo(videoCardName, memory);
	
	fprintf(fp, "Video card information\nvideo card name: %s\ndedicated card memory: %d MB",
		videoCardName, memory);
	
	return true;
}