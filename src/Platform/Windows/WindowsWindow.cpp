#include "pch.h"

#ifdef MW_WINDOWS

#include "src/Window.h"
#include "WindowsKeycode.h"
#include <Windows.h>

namespace mwengine {
	Window* window = nullptr;

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		EventCallback callback = window->GetCallback();
		void* userPtr = window->userPtr;

		switch (uMsg) {
			// Application Event

		case WM_SIZE:
			callback(userPtr, WindowResizeEvent(
				glm::uvec2(
					LOWORD(lParam),
					HIWORD(lParam)
				)
			));

			return 0;
		case WM_SETFOCUS:
			callback(userPtr, WindowFocusEvent());

			return 0;
		case WM_KILLFOCUS:
			callback(userPtr, WindowLostFocusEvent());

			return 0;
		case WM_MOVE:
			callback(userPtr, WindowMovedEvent(
				glm::uvec2(
					LOWORD(lParam),
					HIWORD(lParam)
				)
			));

			return 0;
		case WM_CLOSE:
			callback(userPtr, WindowClosedEvent());

			return 0;

			// Keyboard Events

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			window->_keysDown[WinToMWKey(wParam)] = true;

			callback(userPtr, KeyDownEvent(
				WinToMWKey(wParam),
				(HIWORD(lParam) & KF_REPEAT) == KF_REPEAT
			));

			return 0;
		case WM_KEYUP:
			window->_keysDown[userPtr, WinToMWKey(wParam)] = false;

			callback(userPtr, KeyUpEvent(
				WinToMWKey(wParam)
			));

			return 0;
		case WM_CHAR:
			if (wParam == 0x1B) // do not register escape as KeyCharEvent
				return 0;

			callback(userPtr, KeyCharEvent(
				(char) wParam
			));

			return 0;

			// Mouse Button Pressed Event

		case WM_LBUTTONDOWN:
			window->_mouseDown[MOUSE_LEFT] = true;

			callback(userPtr, MouseDownEvent(
				MOUSE_LEFT
			));

			return 0;
		case WM_RBUTTONDOWN:
			window->_mouseDown[MOUSE_RIGHT] = true;

			callback(userPtr, MouseDownEvent(
				MOUSE_RIGHT
			));

			return 0;
		case WM_MBUTTONDOWN:
			window->_mouseDown[MOUSE_MIDDLE] = true;

			callback(userPtr, MouseDownEvent(
				MOUSE_MIDDLE
			));

			return 0;

			// Mouse Button Released Event

		case WM_LBUTTONUP:
			window->_mouseDown[MOUSE_LEFT] = false;

			callback(userPtr, MouseUpEvent(
				MOUSE_LEFT
			));

			return 0;
		case WM_RBUTTONUP:
			window->_mouseDown[MOUSE_RIGHT] = false;

			callback(userPtr, MouseUpEvent(
				MOUSE_RIGHT
			));

			return 0;
		case WM_MBUTTONUP:
			window->_mouseDown[MOUSE_MIDDLE] = false;

			callback(userPtr, MouseUpEvent(
				MOUSE_MIDDLE
			));

			return 0;

		case WM_MOUSEMOVE:
			window->_mousePos = glm::uvec2(
				LOWORD(lParam),
				HIWORD(lParam)
			);

			callback(userPtr, MouseMovedEvent(
				window->_mousePos
			));

			return 0;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	void DefCallback(void* userPtr, Event& event) {}

	Window::Window(glm::uvec2 size, const std::string& appName, const std::string& title, void* userPtr) {
		window = this;
		this->callback = DefCallback;
		this->userPtr = userPtr;

		RECT rect = {
			0,
			0,
			size.x,
			size.y
		};

		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

		std::wstring wtitle(title.size() + 1, L'#');
		mbstowcs(&wtitle[0], title.c_str(), title.size() + 1);

		std::wstring wAppName(appName.size() + 1, L'#');
		mbstowcs(&wAppName[0], appName.c_str(), appName.size() + 1);

		WNDCLASS wc = {};

		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = wAppName.c_str();
		wc.style = CS_OWNDC;

		MW_ASSERT(RegisterClass(&wc), "Window class failed to register.");

		handle = CreateWindowExW(
			0,
			wAppName.c_str(),
			wtitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right,
			rect.bottom,
			NULL,
			NULL,
			GetModuleHandle(nullptr),
			NULL
		);

		MW_ASSERT(handle != nullptr, "Window failed to create.");

		ShowWindow((HWND) handle, SW_SHOW);
	}

	Window::~Window() {
		DestroyWindow((HWND) handle);
	}

	void Window::Update() {
		window = this;

		MSG msg = {};
		while (PeekMessage(&msg, (HWND) handle, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Window::SetCallback(EventCallback func) {
		window = this;
		callback = func;
	}

	EventCallback Window::GetCallback() {
		return callback;
	}

	glm::uvec2 Window::GetWindowSize() {
		RECT rect;
		GetWindowRect((HWND) handle, &rect);
		return glm::uvec2(rect.right - rect.left, rect.bottom - rect.top);
    }

	glm::uvec2 Window::GetClientSize() {
		RECT rect;
		GetClientRect((HWND) handle, &rect);
		return glm::uvec2(rect.right - rect.left, rect.bottom - rect.top);
    }

	bool Window::GetKeyDown(Keycode keycode) {
		return _keysDown[keycode];
	}

	bool Window::GetMouseDown(MouseCode mousecode) {
		return _mouseDown[mousecode];
	}

	glm::uvec2 Window::GetMousePos() {
		return _mousePos;
	}

	void Window::SetMousePos(glm::uvec2 pos) {
		SetCursorPos(pos.x, pos.y);
	}

    void* Window::GetHandle() {
        return handle;
    }

	void HideConsole() {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	void ShowConsole() {
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	bool IsConsoleVisible() {
		return IsWindowVisible(GetConsoleWindow()) != FALSE;
	}
}

#endif