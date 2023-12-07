#include <clench/wsal/window.h>
#include <chrono>
#include <thread>

using namespace clench::wsal;

int main() {
	std::unique_ptr<Window> window(createWindow(0, L"TH"));
	window->show();

	window->registerEvent<MoveEventCallback>(
		EventType::Move,
		[](int x, int y) {
			printf("Moved to %d, %d\n", x, y);
		});

	while (!window->isClosed()) {
		auto nextFrameTime = std::chrono::system_clock::now() + std::chrono::milliseconds(1000 / 60);

		window->pollEvents();

		while (std::chrono::system_clock::now() < nextFrameTime)
			std::this_thread::yield();
	}
}