#pragma once
#include <comdef.h>

//functions for many aspects of engine
namespace Utils 
{
	static const std::wstring ErrorReport(HRESULT hr)
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		HRESULT hresult = err.Error();
		std::wstringstream wss;
		wss << std::hex;
		wss << "0x" << hresult << std::endl;
		wss << errMsg << std::endl;
		return wss.str();
	}

	static void StringToWString(std::wstring& ws, const std::string& s)
	{
		std::wstring wsTmp(s.begin(), s.end());
		ws = wsTmp;
	}

//#define FAIL_CHECK(hr) if(FAILED(hr)) throw std::exception();
#define FAIL_REPORT(hr) if(FAILED(hr)) MessageBoxEx(nullptr, Utils::ErrorReport(hr).c_str(), L"Detailed Error Report", MB_OK | MB_ICONEXCLAMATION, 0);
}

