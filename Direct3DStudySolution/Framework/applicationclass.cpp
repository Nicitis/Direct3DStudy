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

	// Direct3D 객체를 생성하고 초기화한다.
	m_Direct3D = std::make_unique<D3DClass>();

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not intialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 과제: 텍스트 파일에 그래픽 카드 쓰기
	WriteVideoCardToFile("Video Card Info.txt");

	return true;
}


void ApplicationClass::Shutdown()
{
	// Direct3D 객체를 할당 해제한다.
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

	// 그래픽스 씬을 렌더링한다.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render()
{
	// 씬을 시작하기 위해 버퍼를 초기화한다.
	m_Direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);

	// 렌더링된 씬을 화면 상에 전달한다.
	m_Direct3D->EndScene();

	return true;
}


// 튜토리얼 3 과제 4: 그래픽 카드의 이름과 메모리를 파일에 출력한다.
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