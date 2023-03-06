#include <cassert>
#include "System.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	bool result = false;

	// 시스템 오브젝트 생성
	CSystem* system = new CSystem;
	assert(system);

	// 초기화 및 실행
	result = system->Initialize();
	if (result == true)
	{
		system->Run();
	}

	// 시스템 오브젝트 종료 처리 및 메모리 정리
	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}