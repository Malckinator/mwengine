#pragma once

#include "src/pch.h"
#include "vkShader.h"
#include "vkInstance.h"
#include "vkImageFormat.h"
#include "vkBuffer.h"

namespace mwengine::RenderAPI::Vulkan {
	struct ShaderStage {
		vkShader* shader;
		std::string entry;
	};

	class vkGraphicsPipeline {
	public:
		vkGraphicsPipeline(vkInstance* instance);
		~vkGraphicsPipeline();

		void Rebuild();

		vkBuffer* vertexBuffer;
		std::vector<ShaderDataType> vertexSpecification;
		uint32 vertexCount;
		vkImageFormat* imageFormat;
		ShaderStage vertexShader;
		ShaderStage fragmentShader;

		vkInstance* GetInstance();
		VkPipeline GetNativePipeline();
	private:
		void Destroy();

		vkInstance* instance;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
	};
}