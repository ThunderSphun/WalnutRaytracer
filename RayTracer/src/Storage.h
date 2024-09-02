#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace Storage {
	struct Sphere {
		glm::vec3 pos;
		float radius;
		glm::vec4 albedo;
	};

	struct Scene {
		std::vector<Sphere> spheres;
	};

	struct Ray {
		glm::vec3 origin;
		glm::vec3 direction;
	};
}