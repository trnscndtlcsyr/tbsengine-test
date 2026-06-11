#include "Engine/Core.h"
#include "Engine/WinException.h"
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
	catch (WinException& se)
	{
		MessageBoxEx(nullptr, se.wwhat(), L"Windows Exception", MB_OK | MB_ICONEXCLAMATION, 0);
	}
	catch (std::exception& ex)
	{
		std::wstring errorMsg = WinException::Utf8ToWide(ex.what());
		MessageBoxEx(nullptr, errorMsg.c_str(), L"Standart Exception", MB_OK | MB_ICONEXCLAMATION, 0);
	}
	catch (...)
	{
		MessageBoxEx(nullptr, L"????", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION, 0);
	}
	return 0;
}