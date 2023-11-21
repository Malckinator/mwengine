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
		glm::mat4 result = glm::ortho<float>(-size.x / 2.0f, size.x / 2.0f, -size.y / 2.0f, size.y / 2.0f, -1, 1);
		result[1][1] *= -1;
		return result;
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
		glm::mat4 result = glm::perspective(fov, aspectRatio, 0.0001f, 1000.0f);
		result[1][1] *= -1;
		return result;
    }

    glm::mat4 PerspectiveCamera::GetViewMatrix() {
		return glm::lookAt(pos, pos + (rot * MW_VECTOR_FORWARD), glm::vec3(0, 1, 0));
    }
}