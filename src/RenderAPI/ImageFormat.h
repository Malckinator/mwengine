#pragma once

#include "src/pch.h"

namespace mwengine::RenderAPI {
	class ImageFormat {
	public:
		virtual ~ImageFormat() {};

		virtual glm::uvec2 GetSize() = 0;
	};
}