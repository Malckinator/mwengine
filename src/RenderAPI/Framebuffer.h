#pragma once

#include "src/pch.h"
#include "Instance.h"
#include "Image.h"
#include "Display.h"

namespace mwengine::RenderAPI {
	class Display;

	class Framebuffer {
	public:
		static Framebuffer* Create(Instance* instance, Image* image, Image* depthImage = nullptr);
		static Framebuffer* Create(Instance* instance, Display* display);
		virtual ~Framebuffer() {};

		virtual Instance* GetInstance() = 0;
		virtual bool IsFromDisplay() = 0;
		virtual glm::uvec2 GetSize() = 0;
	};
}