#include "CamController.h"

#include "Walnut/Input/Input.h"

static constexpr glm::vec3 zero(0);

Camera::CamController::CamController() : CamController(1, 1) {
}

Camera::CamController::CamController(float moveSpeed, float rotSpeed) :
	moveSpeed(moveSpeed), rotSpeed(rotSpeed), hasFov(false), camera(nullptr) {
}

Camera::CamController::~CamController() {
	delete camera;
}

const Camera::Cam* Camera::CamController::getCamera() {
	return camera;
}

void Camera::CamController::update(float deltaTime) {
	if (!camera)
		return;

	updatePosition(deltaTime);
	updateRotation(deltaTime);
}

#pragma region getter & setter
const float Camera::CamController::getMoveSpeed() {
	return moveSpeed;
}

void Camera::CamController::setMoveSpeed(float speed) {
	moveSpeed = speed;
}

const float Camera::CamController::getRotSpeed() {
	return rotSpeed;
}

void Camera::CamController::setRotSpeed(float speed) {
	rotSpeed = speed;
}

const glm::vec3& Camera::CamController::getPosition() {
	return camera ? camera->getPosition() : zero;
}

void Camera::CamController::setPosition(const glm::vec3& pos) {
	if (camera)
		camera->setPosition(pos);
}

const glm::vec3& Camera::CamController::getFacing() {
	return camera ? camera->getFacing() : zero;
}

void Camera::CamController::setFacing(const glm::vec3& facing) {
	if (camera)
		camera->setFacing(facing);
}

void Camera::CamController::setSize(float width, float height) {
	if (camera)
		camera->setSize(width, height);
}

void Camera::CamController::setSize(float width, float height, float near, float far) {
	if (camera)
		camera->setSize(width, height, near, far);
}

void Camera::CamController::setPlanes(float near, float far) {
	if (camera)
		camera->setPlanes(near, far);
}

const float Camera::CamController::getFov() {
	return (camera && hasFov) ? static_cast<PerspectiveCam*>(camera)->getFov() : 0;
}

void Camera::CamController::setFov(float fovY) {
	if (camera && hasFov)
		static_cast<PerspectiveCam*>(camera)->setFov(fovY);
}
#pragma endregion

void Camera::CamController::updatePosition(float deltaTime) {
	glm::vec3 movedDir(0);

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
		movedDir.z++;
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
		movedDir.z--;

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
		movedDir.x--;
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
		movedDir.x++;

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Space))
		movedDir.y--;
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::LeftShift))
		movedDir.y++;

	if (movedDir == glm::vec3(0))
		return;

	movedDir *= moveSpeed * deltaTime;

	glm::vec3 camPos = camera->getPosition();
	glm::vec3 camDir = camera->getFacing();

	const glm::vec3 fakeUp(0, 1, 0);
	glm::vec3 right = glm::cross(camDir, fakeUp);
	glm::vec3 up = glm::cross(camDir, right);

	glm::vec3 nextCamPos = camPos + movedDir.x * right + movedDir.y * up + movedDir.z * camDir;

	camera->setPosition(nextCamPos);
}

void Camera::CamController::updateRotation(float deltaTime) {

}