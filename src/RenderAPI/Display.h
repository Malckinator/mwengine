#pragma once

#include "Instance.h"
#include "ImageFormat.h"

namespace mwengine::RenderAPI::Vulkan {
	class Display {
	public:
		static Display* Create(Instance* instance, Window* window = nullptr);
		virtual ~Display() {};

		virtual void Rebuild() = 0;

		virtual Instance* GetInstance() = 0;
		virtual ImageFormat* GetImageFormat() = 0;
	};
}