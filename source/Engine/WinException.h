#pragma once

#include <comdef.h>>
#include <system_error>
#include <source_location>
#include <sstream>

class WinException : public std::system_error {
public:
	WinException(HRESULT hr, const std::string& expr, const std::source_location& loc);
	const wchar_t* wwhat() const noexcept { return w_message.c_str(); }
	static inline std::wstring Utf8ToWide(const std::string& str);
private:
	std::wstring w_message;
};

// DX validator (HRESULT)
#define DX_CHECK(expr) \
    do { \
        HRESULT hr__ = (expr); \
        if (FAILED(hr__)) { \
            throw WinException(hr__, #expr, std::source_location::current()); \
        } \
    } while(0)

// WinAPI validator (bool/BOOL & GetLastError)
#define WIN_CHECK(expr) \
    do { \
        if (!(expr)) { \
            throw WinException(HRESULT_FROM_WIN32(GetLastError()), #expr, std::source_location::current()); \
        } \
    } while(0)

