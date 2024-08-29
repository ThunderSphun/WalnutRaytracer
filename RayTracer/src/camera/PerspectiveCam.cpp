#include "PerspectiveCam.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::PerspectiveCam::PerspectiveCam(float fovY, float near, float far, float width, float height)
	: Cam::Cam(near, far, width, height), fov(fovY) {
}

const float Camera::PerspectiveCam::getFov() {
	return fov;
}

const float Camera::PerspectiveCam::getFov() const {
	return fov;
}

void Camera::PerspectiveCam::setFov(float fovY) {
	if (fov == fovY)
		return;
	fov = fovY;

	calculateProjection();
	calculateRayCache();
}

void Camera::PerspectiveCam::calculateRayCache() {
	rayCache.resize((int) size.x * (int) size.y);

	for (size_t y = 0; y < size.y; y++) {
		for (size_t x = 0; x < size.x; x++) {
			glm::vec2 coord = glm::vec2(x / size.x, y / size.y) * 2.0f - 1.0f;

			glm::vec4 target = inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			glm::vec3 rayDirection = glm::vec3(inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
			Storage::Ray ray;
			ray.direction = rayDirection;
			ray.origin = position + rayDirection * nearPlane;
			rayCache[x + y * (int) size.x] = ray;
		}
	}
}

void Camera::PerspectiveCam::calculateProjection() {
	projection = glm::perspective(glm::radians(fov), size.x / size.y, nearPlane, farPlane);
	inverseProjection = glm::inverse(projection);
}

void Camera::PerspectiveCam::calculateView() {
	view = glm::lookAt(position, position + lookAngles.forward, lookAngles.up);
	inverseView = glm::inverse(view);
}