#ifndef _CLENCH_GRAPHICS_WIN32_WINDOW_H_
#define _CLENCH_GRAPHICS_WIN32_WINDOW_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <clench/wsal/window.h>

#include <map>
#include <set>
#include <string>

namespace clench {
	namespace wsal {
		class Win32Window : public Window {
		protected:
			HWND _hWnd;
			bool _isClosed = false;
			std::set<UINT> _keyboardPressedKeys;

		public:
			static LRESULT _wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			Win32Window(CreateWindowFlags flags, std::wstring title);
			virtual ~Win32Window();
			virtual void pollEvents() override;

			virtual void show() override;
			virtual void hide() override;

			virtual bool isVisible() const override;

			virtual inline bool isClosed() const { return _isClosed; }

			virtual long long getSize() const override;
			virtual void setSize(int width, int height) override;

			virtual unsigned int getSystemDpi() const override;

			virtual void* getNativeHandle() const override;

			virtual WindowBackend& getBackend() const override;

			virtual bool isKeyDown(KeyboardKeyCode keyCode) const override;
		};

		class Win32WindowBackend : public WindowBackend {
		public:
			Win32WindowBackend();
			virtual ~Win32WindowBackend();

			virtual Window* createWindow(CreateWindowFlags flags, std::wstring title) override;
		};

		extern std::map<UINT, KeyboardKeyCode> win32KeyMap;
	}
}

#endif
