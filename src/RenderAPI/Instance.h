#pragma once

#include "src/pch.h"
#include "src/Window.h"
#include "PhysicalDevice.h"

namespace mwengine::RenderAPI {
	enum RenderAPI : uint8 {
		API_UNKNOWN,
		API_VULKAN
	};

	class Instance {
	public:
		static Instance* Create(RenderAPI renderApi, Window* window, const std::string& appName = "app", uint32 appVersion = MW_MAKE_VERSION(1, 0, 0), bool debug = true);
		virtual ~Instance() {};

		RenderAPI GetRenderAPI();
		virtual std::vector<PhysicalDevice*> GetPhysicalDevices() = 0;
		virtual PhysicalDevice* GetOptimalPhysicalDevice() = 0;
		virtual void SetPhysicalDevice(PhysicalDevice* physicalDevice) = 0;
		virtual void WaitUntilIdle() = 0;
		virtual Window* GetWindow() = 0;
		virtual PhysicalDevice* GetPhysicalDevice() = 0;
	private:
		RenderAPI renderApi;
	};
}