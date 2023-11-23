#pragma once

#include "src/pch.h"
#include "Display.h"
#include "GraphicsPipeline.h"

namespace mwengine::RenderAPI {
	class CommandBuffer {
	public:
		static CommandBuffer* Create(Instance* instance);
		virtual ~CommandBuffer() {};

		virtual void StartFrame(Display* display) = 0;
		virtual void EndFrame() = 0;

		virtual void QueueDraw(
			GraphicsPipeline* graphicsPipeline,
			BufferRegion vertexBuffer = {},
			BufferRegion indexBuffer = {},
			BufferRegion instanceBuffer = {},
			BufferRegion uniformBuffer = {},
			std::vector<Texture*> textures = {}
		) = 0;
	};
}