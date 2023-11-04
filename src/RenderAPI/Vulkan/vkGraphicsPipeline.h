#pragma once

#include "src/pch.h"
#include "vkShader.h"
#include "vkInstance.h"
#include "vkImageFormat.h"

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

		vkImageFormat* imageFormat;
		ShaderStage vertexShader;
		ShaderStage fragmentShader;
		uint32 vertexCount;

		vkInstance* GetInstance();
		uint32 GetVertexCount();
		VkPipeline GetNativePipeline();
	private:
		void Destroy();

		vkInstance* instance;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		uint32 _vertexCount;
		vkImageFormat* _imageFormat;
	};
}