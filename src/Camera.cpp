#include "pch.h"
#include "Camera.h"

namespace mwengine {
	Camera::~Camera() {

	}

	OrthographicCamera::OrthographicCamera(glm::vec3 pos, glm::vec2 size, float rot) {
		this->pos = pos;
		this->size = size;
		this->rot = rot;
	}

	OrthographicCamera::~OrthographicCamera() {

	}

	glm::mat4 OrthographicCamera::GetProjectionMatrix() {
		return glm::ortho<float>(-size.x / 2.0f, size.x / 2.0f, -size.y / 2.0f, size.y / 2.0f, -1, 1);
	}

	glm::mat4 OrthographicCamera::GetViewMatrix() {
		glm::mat4 transform = glm::translate(glm::mat4(1), pos) * glm::rotate(glm::mat4(1), rot, glm::vec3(0, 0, 1));
		return glm::inverse(transform);
	}

    PerspectiveCamera::PerspectiveCamera(float aspectRatio, float fov) {
		this->aspectRatio = aspectRatio;
		this->fov = fov;
		this->pos = {};
		this->rot = {};
    }

    PerspectiveCamera::~PerspectiveCamera() {

    }

    glm::mat4 PerspectiveCamera::GetProjectionMatrix() {
		glm::mat4 result = glm::perspective(fov, aspectRatio, 0.0f, 10.0f);
		result[1][1] *= -1;
		return result;
    }

    glm::mat4 PerspectiveCamera::GetViewMatrix() {
		glm::mat4 result = glm::mat4(1);
		result *= glm::translate(glm::mat4(1), pos);
		//result *= glm::rotate(glm::mat4(1), rot.z, glm::vec3(0, 0, 1));
		//result *= glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		//result *= glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
		result *= glm::mat4_cast(rot);
		return glm::inverse(result);
    }
}