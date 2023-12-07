#include "window.h"

#include <comdef.h>

#include <locale>
#include <map>
#include <stdexcept>

using namespace clench;
using namespace clench::wsal;

static Win32WindowBackend _backend;

static WNDCLASSW _wndClass;
constexpr LPCWSTR WNDCLASS_NAME = L"CLENCHMAIN";
std::map<HWND, Win32Window*> _createdWindows;
static bool _inited = false;

static void _initWindow() {
	if (FAILED(CoInitialize(0)))
		throw std::runtime_error("Unable to initialize the COM");

	_wndClass.lpszClassName = WNDCLASS_NAME;
	_wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	_wndClass.hCursor = LoadIcon((HINSTANCE)GetModuleHandle(NULL), IDC_ARROW);
	_wndClass.hInstance = GetModuleHandle(NULL);
	_wndClass.lpfnWndProc = Win32Window::_wndProc;
	_wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	if (!RegisterClassW(&_wndClass)) {
		CoUninitialize();
		throw std::runtime_error("Unable to register window class, last error = " + std::to_string(GetLastError()));
	}

	SetProcessDPIAware();  // Disable DPI scaling
}

clench::wsal::Win32Window::Win32Window(CreateWindowFlags flags, std::wstring title) {
	if (!(_hWnd = CreateWindowExW(
			  WS_EX_OVERLAPPEDWINDOW,
			  WNDCLASS_NAME,
			  title.c_str(),
			  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  DEFAULT_WINDOW_WIDTH,
			  DEFAULT_WINDOW_HEIGHT,
			  NULL,
			  NULL,
			  GetModuleHandle(nullptr),
			  NULL))) {
		throw std::runtime_error("Error creating window, last error = " + std::to_string(GetLastError()));
	}
	_createdWindows[_hWnd] = this;
}

clench::wsal::Win32Window::~Win32Window() {
	_createdWindows.erase(_hWnd);
}

void clench::wsal::Win32Window::pollEvents() {
	MSG msg;
	while (PeekMessage(&msg, _hWnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void clench::wsal::Win32Window::show() {
	ShowWindow(_hWnd, SW_SHOW);
}

void clench::wsal::Win32Window::hide() {
	ShowWindow(_hWnd, SW_HIDE);
}

bool clench::wsal::Win32Window::isVisible() const {
	return (bool)IsWindowVisible(_hWnd);
}

long long clench::wsal::Win32Window::getSize() const {
	RECT rect;
	GetClientRect(_hWnd, &rect);
	return (((long long)(rect.right - rect.left)) << 32) | (rect.bottom - rect.top);
}

void clench::wsal::Win32Window::setSize(int width, int height) {
	RECT rect;
	GetClientRect(_hWnd, &rect);
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	AdjustWindowRect(&rect, 0, false);
}

void* clench::wsal::Win32Window::getNativeHandle() const {
	return (void*)&_hWnd;
}

unsigned int clench::wsal::Win32Window::getSystemDpi() const {
	return GetDpiForSystem();
}

LRESULT clench::wsal::Win32Window::_wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (auto it = _createdWindows.find(hWnd); it != _createdWindows.end()) {
		auto window = it->second;

		switch (uMsg) {
			case WM_SHOWWINDOW:
				break;
			case WM_SIZE:
				if (auto it = window->_events.find(EventType::Resize); it != window->_events.end()) {
					it->second->call<ResizeEventCallback>(LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_MOVE:
				if (auto it = window->_events.find(EventType::Move); it != window->_events.end()) {
					it->second->call<MoveEventCallback>(LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_CLOSE:
				if (auto it = window->_events.find(EventType::Close); it != window->_events.end()) {
					if (it->second->call<CloseEventCallback>())
						break;
				}
			case WM_DESTROY:
				window->_isClosed = true;
				break;
			case WM_KEYDOWN: {
				auto key = (UINT)wParam;

				if (win32KeyMap.count(key)) {
					window->_keyboardPressedKeys.insert(win32KeyMap.at(key));
				} else
					window->_keyboardPressedKeys.insert(key);
				break;
			}
			case WM_KEYUP: {
				auto key = (UINT)wParam;

				if (win32KeyMap.count(key)) {
					window->_keyboardPressedKeys.erase(win32KeyMap.at(key));
				} else
					window->_keyboardPressedKeys.erase(key);
				break;
			}
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WindowBackend& Win32Window::getBackend() const {
	return _backend;
}

bool clench::wsal::Win32Window::isKeyDown(KeyboardKeyCode keyCode) const {
	return _keyboardPressedKeys.count(keyCode);
}

Win32WindowBackend::Win32WindowBackend() : WindowBackend("Win32") {
}

Win32WindowBackend::~Win32WindowBackend() {
	UnregisterClassW(WNDCLASS_NAME, NULL);
	CoUninitialize();
}

wsal::Window* Win32WindowBackend::createWindow(CreateWindowFlags flags, std::wstring title) {
	if (!_inited)
		_initWindow();
	return new Win32Window(flags, title);
}
