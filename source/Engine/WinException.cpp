#include "WinException.hpp"
#include <string>
#include <format>

WinException::WinException(
	HRESULT hr,
	const std::string& expr,
	const std::source_location& loc)
	:
	std::system_error(hr, std::system_category())
{

	std::wstring wFile = Utf8ToWide(loc.file_name());
	std::wstring wFunc = Utf8ToWide(loc.function_name());
	std::wstring wExpr = Utf8ToWide(expr);
	std::wstring wDesc = Utf8ToWide(std::system_error::what());
	w_message = std::format(
		L"[WinAPI/DX Error]\nExpression: {}\nFile: {}\nLine: {}\nFunction: {}\nDescription: {}",
		wExpr, wFile, loc.line(), wFunc, wDesc);
}


inline std::wstring WinException::Utf8ToWide(const std::string& str) {
	if (str.empty()) return {};
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
	std::wstring wstr(sizeNeeded, 0); 
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], sizeNeeded);
	return wstr;
}