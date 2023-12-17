#pragma once

#include "Instance.h"
#include "Framebuffer.h"

namespace mwengine::RenderAPI {
	class Display {
	public:
		static Display* Create(Instance* instance, Window* window = nullptr);
		virtual ~Display() {};

		virtual Instance* GetInstance() = 0;
		virtual Framebuffer* GetFramebuffer() = 0;
	};
}