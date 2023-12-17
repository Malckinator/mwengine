#pragma once

#include "src/pch.h"
#include "Shader.h"
#include "Instance.h"
#include "Buffer.h"
#include "Image.h"
#include "Framebuffer.h"

namespace mwengine::RenderAPI {
	struct ShaderStage {
		Shader* shader;
		std::string entry;
	};

	enum CullingMode {
		CULLING_MODE_NONE = 0,
		CULLING_MODE_FRONT_BIT = MW_BIT(0),
		CULLING_MODE_BACK_BIT = MW_BIT(1),
		CULLING_MODE_FRONT_AND_BACK = CULLING_MODE_FRONT_BIT | CULLING_MODE_BACK_BIT
	};

	class GraphicsPipeline {
	public:
		static GraphicsPipeline* Create(Instance* instance);
		virtual ~GraphicsPipeline() {};

		virtual void Rebuild() = 0;
		virtual void SetDefaults() = 0;
		virtual Instance* GetInstance() = 0;

		Framebuffer* framebuffer;
		std::vector<ShaderDataType> vertexSpecification;
		uint32 indexCount;
		std::vector<ShaderDataType> instanceSpecification;
		uint32 instanceCount;
		bool hasUniformBuffer;
		uint32 textureCount;
		CullingMode cullingMode;
		bool depthTesting;
		ShaderStage vertexShader;
		ShaderStage fragmentShader;
	};
}