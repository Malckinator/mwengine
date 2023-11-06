#pragma once

#include "pch.h"
#include "Event.h"
#include <string>

namespace mwengine {
	class Window {
	public:
		Window(glm::uvec2 size, const std::string& appName, const std::string& title, void* userPtr = nullptr);
		~Window();

		void Update();
		void SetCallback(EventCallback func);
		EventCallback GetCallback();

		glm::uvec2 GetWindowSize();
		glm::uvec2 GetClientSize();
		bool GetKeyDown(Keycode keycode);
		bool GetMouseDown(MouseCode mousecode);
		glm::uvec2 GetMousePos();
		void SetMousePos(glm::uvec2 pos);

		void* GetHandle();

		void* userPtr;
		bool keysDown[Keycode::Last + 1];
		bool mouseDown[MouseCode::Last + 1];
		glm::uvec2 mousePos;
	private:
		EventCallback callback;
		void* handle;
	};
}