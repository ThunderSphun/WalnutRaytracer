#include "Cam.h"

#include <float.h>

Camera::Cam::Cam(float near, float far, float width, float height)
	: nearPlane(near), farPlane(far), size(width, height),
	projection(1), inverseProjection(glm::inverse(projection)),
	view(1), inverseView(glm::inverse(view)), position(NAN),
	lookAngles(Camera::Cam::LookAngles::fromFacing(glm::vec3(0))) {
}

#pragma region getter & setter
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

void Camera::Cam::setPosition(const glm::vec3& pos) {
	if (position == pos)
		return;
	position = pos;

	calculateProjection();
	calculateView();
	calculateRayCache();
}

const glm::vec3& Camera::Cam::getFacing() {
	return lookAngles.forward;
}

const glm::vec3& Camera::Cam::getFacing() const {
	return lookAngles.forward;
}

const Camera::Cam::LookAngles& Camera::Cam::getLookAngles() {
	return lookAngles;
}

const Camera::Cam::LookAngles& Camera::Cam::getLookAngles() const {
	return lookAngles;
}

void Camera::Cam::setFacing(const glm::vec3& facing) {
	glm::vec3 nextLookDir= glm::normalize(facing);
	if (lookAngles.forward == nextLookDir)
		return;

	lookAngles = Camera::Cam::LookAngles::fromFacing(facing);

	calculateProjection();
	calculateView();
	calculateRayCache();
}

const glm::vec2& Camera::Cam::getSize() {
	return size;
}

const glm::vec2& Camera::Cam::getSize() const {
	return size;
}

const glm::vec2& Camera::Cam::getPlanes() {
	static glm::vec2 ret(0, 0);
	ret = glm::vec2(nearPlane, farPlane);
	return ret;
}

const glm::vec2& Camera::Cam::getPlanes() const {
	static glm::vec2 ret(0, 0);
	ret = glm::vec2(nearPlane, farPlane);
	return ret;
}

void Camera::Cam::setSize(float width, float height) {
	glm::vec2 nextSize(width, height);
	if (size == nextSize)
		return;
	size = nextSize;

	calculateProjection();
	calculateView();
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

	calculateProjection();
	calculateView();
	calculateRayCache();
}
#pragma endregion

const Camera::Cam::LookAngles Camera::Cam::LookAngles::fromFacing(const glm::vec3& facing) {
	const glm::vec3 forward = glm::normalize(facing);
	glm::vec3 fakeUp(0, 1, 0);
	if (fakeUp == glm::abs(forward))
		fakeUp = glm::vec3(1, 0, 0);
	const glm::vec3 right = glm::cross(forward, fakeUp);
	const glm::vec3 up = glm::cross(forward, right);

	return {forward, up, right};
}