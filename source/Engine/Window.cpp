#include "Window.h"
#include "WinException.h"

void Window::InitWindowClass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = MsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpszClassName = class_name;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.hIconSm = nullptr;

	WIN_CHECK(RegisterClassEx(&wc));
}

Window::Window(int width, int height, const wchar_t* name) : width(width), height(height)
{
	InitWindowClass();
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	WIN_CHECK(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE));
	hWnd = CreateWindowEx(
		0,
		class_name,
		name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr,
		nullptr,
		hInstance,
		this
	);
	WIN_CHECK(hWnd);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	pRenderer = std::make_unique<GraphicsComponent>(hWnd);
}

Window::~Window()
{
	UnregisterClass(class_name, hInstance);
	DestroyWindow(hWnd);
}

int Window::MessageProcessing()
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 1;
}

GraphicsComponent& Window::gfx()
{
	WIN_CHECK(pRenderer);
	return *pRenderer;
}

void Window::OnResize(UINT w, UINT h)
{
	width = w;
	height = h;
	if (pRenderer.get()) {
		pRenderer->RenderResize(w, h);
	}
}

//сетап до создания окна, где происходит подмена сетапа MsgSetup на MsgProcReplace и копируются данные текущего окна
LRESULT WINAPI Window::MsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::MsgProcReplace));
		return pWnd->HandleMsg(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//подмена для сетапа wndProc, чтобы вызывать конкретный не статик метод, конкретного обьекта
LRESULT WINAPI Window::MsgProcReplace(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, uMsg, wParam, lParam);
}

//основная машина сообщение
LRESULT Window::HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			OnResize(width, height);
		}
		break;
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			if (pt.x >= 0 && pt.x < width && 
				pt.y >= 0 && pt.y < height) {
				mouse.OnMouseMove(pt.x, pt.y);
			}
			break;
		}
		case WM_MBUTTONDOWN:
		{
			mouse.OnWheelPressed();
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
		case WM_MBUTTONUP:
		{
			mouse.OnWheelReleased();
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
		case WM_LBUTTONUP:
		{
			mouse.OnLMBReleased();
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			mouse.OnLMBPressed();
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
		case WM_RBUTTONUP:
		{
			mouse.OnRMBReleased();
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			mouse.OnRMBPressed();
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(delta);
			const POINTS pt = MAKEPOINTS(lParam);
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Window::TextPost(HWND hWnd, const POINTS pt, const char* text)
{
	std::wstringstream wss;
	wss << text << pt.x << "," << pt.y;
	SetWindowText(hWnd, wss.str().c_str());
}
