#include <cassert>
#include "System.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	bool result = false;

	// �ý��� ������Ʈ ����
	CSystem* system = new CSystem;
	assert(system);

	// �ʱ�ȭ �� ����
	result = system->Initialize();
	if (result == true)
	{
		system->Run();
	}

	// �ý��� ������Ʈ ���� ó�� �� �޸� ����
	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}