#include "pch.h"
#include "Transform.h"

namespace mwengine {
	Transform::Transform(glm::vec3 pos, glm::vec3 size, glm::vec3 rot) {
		this->pos = pos;
		this->size = size;
		this->rot = rot;
	}

	glm::mat4 Transform::GetMatrix() {
		glm::mat4 rotation = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0))
			* glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0))
			* glm::rotate(glm::mat4(1), rot.z, glm::vec3(0, 0, 1));

		return glm::translate(glm::mat4(1), pos) * rotation * glm::scale(glm::mat4(1), size);
	}
}