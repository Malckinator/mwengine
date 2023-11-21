#include "pch.h"
#include "Transform.h"

namespace mwengine {
	Transform::Transform(glm::vec3 pos, glm::vec3 size, glm::quat rot) {
		this->pos = pos;
		this->size = size;
		this->rot = rot;
	}

	glm::mat4 Transform::GetMatrix() {
		return glm::translate(glm::mat4(1), pos) * glm::mat4_cast(rot) * glm::scale(glm::mat4(1), size);
	}
}