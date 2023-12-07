#ifndef _CLENCH_GRAPHICS_EVENT_H_
#define _CLENCH_GRAPHICS_EVENT_H_

#include <functional>
#include <cstdint>
#include <type_traits>

namespace clench {
	namespace wsal {
		enum class EventType : uint16_t {
			Show = 0,
			Hide,
			Resize,
			Move,
			Close
		};

		class EventCallback {
		public:
			void* ptr;

			inline EventCallback(void* ptr) : ptr(ptr) {}
			virtual ~EventCallback() = default;

			template<typename T, typename ...Args>
			inline std::invoke_result_t<T, Args...> call(Args ...args) {
				return (*(T*)ptr)(args...);
			}
		};

		template<typename T>
		class EventCallbackImpl : public EventCallback {
		public:
			inline EventCallbackImpl(T callback) : EventCallback(new T(callback)) {
			}

			virtual ~EventCallbackImpl() {
				delete ((T*)ptr);
			}
		};

		using ResizeEventCallback = std::function<void(int width, int height)>;
		using MoveEventCallback = std::function<void(int x, int y)>;
		using CloseEventCallback = std::function<bool()>;
	}
}

#endif
