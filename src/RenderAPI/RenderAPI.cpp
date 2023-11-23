#include "pch.h"
#include "Instance.h"
#include "Vulkan/vkInstance.h"

#define GET_API_CONSTRUCTOR(type, renderApi, ...) if (renderApi == API_VULKAN) return (type*) new vk##type##(__VA_ARGS__); else MW_ERROR("Render api id: {0} not supported.", renderApi);

using namespace mwengine::RenderAPI::Vulkan;

namespace mwengine::RenderAPI {
	Instance* Instance::Create(RenderAPI renderApi, Window* window, const std::string& appName, uint32 appVersion, bool debug) {
		GET_API_CONSTRUCTOR(Instance, renderApi, window, appName, appVersion, debug);
	}

	RenderAPI Instance::GetRenderAPI() {
		return renderApi;
	}
}