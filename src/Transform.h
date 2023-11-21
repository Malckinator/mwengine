#pragma once

#include "src/pch.h"

namespace mwengine {
	class Transform {
	public:
		Transform(glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 size = glm::vec3(1, 1, 1), glm::quat rot = glm::fquat(glm::vec3(0, 0, 0)));

		glm::mat4 GetMatrix();

		glm::vec3 pos;
		glm::vec3 size;
		glm::quat rot;
	};
}