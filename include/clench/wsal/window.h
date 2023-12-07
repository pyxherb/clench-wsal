#ifndef _CLENCH_GRAPHICS_WINDOW_H_
#define _CLENCH_GRAPHICS_WINDOW_H_

#include "event.h"
#include "keyboard.h"
#include <deque>
#include <memory>
#include <string>
#include <map>

namespace clench {
	namespace wsal {
		using CreateWindowFlags = std::uint32_t;
		constexpr CreateWindowFlags
			CREATEWINDOW_MIN = 1,
			CREATEWINDOW_MAX = 2,
			CREATEWINDOW_RESIZE = 4,
			CREATEWINDOW_NOFRAME = 8;

		constexpr int DEFAULT_WINDOW_WIDTH = 1280, DEFAULT_WINDOW_HEIGHT = 960, DEFAULT_DPI = 72;

		class WindowBackend;

		class Window {
		protected:
			std::map<EventType, std::unique_ptr<EventCallback>> _events;

		public:
			virtual inline ~Window(){};

			virtual void pollEvents() = 0;

			template<typename T>
			inline void registerEvent(EventType type, T callback) {
				_events[type] = std::make_unique<EventCallbackImpl<T>>(callback);
			}

			template<typename T>
			inline T& getEvent(EventType type) {
				return *((T*)_events.at(type)->ptr);
			}

			virtual void show() = 0;
			virtual void hide() = 0;
			virtual bool isVisible() const = 0;

			virtual bool isClosed() const = 0;

			virtual long long getSize() const = 0;
			virtual void setSize(int width, int height) = 0;

			virtual unsigned int getSystemDpi() const = 0;

			virtual void* getNativeHandle() const = 0;

			virtual WindowBackend& getBackend() const = 0;

			virtual bool isKeyDown(KeyboardKeyCode) const = 0;
		};

		class WindowBackend {
		private:
			std::string _name;

		public:
			WindowBackend(std::string name);
			virtual ~WindowBackend() = default;

			virtual Window* createWindow(CreateWindowFlags flags, std::wstring title) = 0;

			inline std::string getName() { return _name; }
		};

		Window* createWindow(
			CreateWindowFlags flags,
			std::wstring title,
			std::deque<std::string> preferredBackends = {});
	}
}

#endif
