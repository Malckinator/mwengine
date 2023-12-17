#pragma once

#include "src/pch.h"
#include "Instance.h"

namespace mwengine::RenderAPI {
	class Framebuffer {
	public:
		virtual ~Framebuffer() {};

		virtual Instance* GetInstance() = 0;
		virtual bool IsFromDisplay() = 0;
		virtual glm::uvec2 GetSize() = 0;
	};
}