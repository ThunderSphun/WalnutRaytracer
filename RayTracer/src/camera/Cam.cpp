#include "Cam.h"

Camera::Cam::Cam(float near, float far, float width, float height) : nearPlane(near), farPlane(far), size(width, height),
	projection(0), inverseProjection(glm::inverse(projection)), view(0), inverseView(glm::inverse(view)), position(0), lookdir(0) {
}

const std::vector<Storage::Ray>& Camera::Cam::getRays() {
	return rayCache;
}

const std::vector<Storage::Ray>& Camera::Cam::getRays() const {
	return rayCache;
}

const glm::mat4& Camera::Cam::getProjection() {
	return projection;
}

const glm::mat4& Camera::Cam::getInverseProjection() {
	return inverseProjection;
}

const glm::mat4& Camera::Cam::getView() {
	return view;
}

const glm::mat4& Camera::Cam::getInverseView() {
	return inverseView;
}

const glm::mat4& Camera::Cam::getProjection() const {
	return projection;
}

const glm::mat4& Camera::Cam::getInverseProjection() const {
	return inverseProjection;
}

const glm::mat4& Camera::Cam::getView() const {
	return view;
}

const glm::mat4& Camera::Cam::getInverseView() const {
	return inverseView;
}

const glm::vec3& Camera::Cam::getPosition() {
	return position;
}

const glm::vec3& Camera::Cam::getPosition() const {
	return position;
}

void Camera::Cam::setPosition(glm::vec3 pos) {
	if (position == pos)
		return;
	position = pos;

	calculateView();
	calculateRayCache();
}

const glm::vec3& Camera::Cam::getFacing() {
	return lookdir;
}

const glm::vec3& Camera::Cam::getFacing() const {
	return lookdir;
}

void Camera::Cam::setFacing(glm::vec3 facing) {
	facing = glm::normalize(facing);
	if (lookdir == facing)
		return;
	lookdir = facing;

	calculateView();
	calculateRayCache();
}

void Camera::Cam::setSize(float width, float height) {
	glm::vec2 newSize(width, height);
	if (size == newSize)
		return;
	size = newSize;

	calculateProjection();
	calculateRayCache();
}

void Camera::Cam::setSize(float width, float height, float near, float far) {
	glm::vec2 newSize(width, height);
	if (size == newSize && nearPlane == near && farPlane == far)
		return;
	size = newSize;
	nearPlane = near;
	farPlane = far;

	calculateProjection();
	calculateView();
	calculateRayCache();
}

void Camera::Cam::setPlanes(float near, float far) {
	if (nearPlane == near && farPlane == far)
		return;
	nearPlane = near;
	farPlane = far;

	calculateView();
	calculateRayCache();
}