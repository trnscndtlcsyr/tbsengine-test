#include "Engine/Core.h"
#include "Engine/Utils.h"
#include "Application.h"

int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ PWSTR pCnmdLine, 
	_In_ int nCmdShow)
{
	try 
	{
		Application app{};
		app.Run();
	}
	catch (std::exception& ex)
	{
		std::wstring errorMsg;
		Utils::StringToWString(errorMsg, ex.what());
		MessageBoxEx(nullptr, errorMsg.c_str(), L"Standart Exception", MB_OK | MB_ICONEXCLAMATION, 0);
	}
	catch (...)
	{
		MessageBoxEx(nullptr, L"????", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION, 0);
	}
	MessageBoxEx(
		nullptr,
		std::to_wstring(_CrtDumpMemoryLeaks()).c_str(),
		L"MEMORY LEAK NOTIFICATION",
		MB_OK | MB_ICONQUESTION,
		0
	);
	return 0;
}