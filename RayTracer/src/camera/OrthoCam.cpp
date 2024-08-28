#include "OrthoCam.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::OrthoCam::OrthoCam(float near, float far, float width, float height) : Cam::Cam(near, far, width, height) {
}

void Camera::OrthoCam::calculateRayCache() {
	rayCache.resize((int)size.x * (int)size.y);

	const glm::vec3 fakeUp(0, 1, 0);
	glm::vec3 right = glm::cross(lookdir, fakeUp);
	glm::vec3 up = glm::cross(lookdir, right);

	float ratio = size.x / size.y;

	for (size_t y = 0; y < size.y; y++) {
		for (size_t x = 0; x < size.x; x++) {
			glm::vec2 coord = glm::vec2(x / size.x, y / size.y) * 2.0f - 1.0f;
			coord.x *= ratio;

			Storage::Ray ray;
			ray.origin = position + coord.x * right + coord.y * up + lookdir * nearPlane;
			ray.direction = lookdir;
			rayCache[x + y * (int)size.x] = ray;
		}
	}
}

void Camera::OrthoCam::calculateProjection() {
	projection = glm::ortho(-size.x / 2, size.x / 2, -size.y / 2, size.y / 2, nearPlane, farPlane);
	inverseProjection = glm::inverse(projection);
}

void Camera::OrthoCam::calculateView() {
	view = glm::lookAt(position, position + lookdir, glm::vec3(0, 1, 0));
	inverseView = glm::inverse(view);
}