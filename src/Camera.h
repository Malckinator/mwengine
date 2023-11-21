#include "src/pch.h"
#include "Window.h"

namespace mwengine {
	class Camera {
	public:
		virtual ~Camera();

		virtual glm::mat4 GetProjectionMatrix() = 0;
		virtual glm::mat4 GetViewMatrix() = 0;
	};

	class OrthographicCamera : Camera {
	public:
		OrthographicCamera(glm::vec3 pos, glm::vec2 size, float rot);
		virtual ~OrthographicCamera();

		virtual glm::mat4 GetProjectionMatrix();
		virtual glm::mat4 GetViewMatrix();

		glm::vec3 pos;
		glm::vec2 size;
		float rot;
	};

	class PerspectiveCamera : Camera {
	public:
		PerspectiveCamera(float aspectRatio = 1.0f, float fov = glm::radians(45.0f));
		virtual ~PerspectiveCamera();

		virtual glm::mat4 GetProjectionMatrix();
		virtual glm::mat4 GetViewMatrix();

		float aspectRatio;
		float fov;
		glm::vec3 pos;
		glm::quat rot;
	};
}