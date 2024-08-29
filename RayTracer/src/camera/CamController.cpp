#include "CamController.h"

#include "Walnut/Input/Input.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

static constexpr glm::vec3 zero(0);

Camera::CamController::CamController() : CamController(1, 1) {
}

Camera::CamController::CamController(float moveSpeed, float rotSpeed) :
	moveSpeed(moveSpeed), rotSpeed(rotSpeed), hasFov(false), camera(nullptr), prevMousePos(0) {
}

Camera::CamController::~CamController() {
	delete camera;
}

const Camera::Cam* Camera::CamController::getCamera() {
	return camera;
}

void Camera::CamController::update(float deltaTime) {
	glm::vec2 mousePos = Walnut::Input::GetMousePosition();
	glm::vec2 mouseDelta = (mousePos - prevMousePos) * 0.2f;
	prevMousePos = mousePos;

	if (!camera)
		return;

	if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right)) {
		Walnut::Input::SetCursorMode(Walnut::CursorMode::Locked);
		updatePosition(deltaTime);
		updateRotation(mouseDelta, deltaTime);
	} else
		Walnut::Input::SetCursorMode(Walnut::CursorMode::Normal);
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
		movedDir.x++;
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
		movedDir.x--;

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Space))
		movedDir.y++;
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::LeftShift))
		movedDir.y--;

	if (movedDir == glm::vec3(0))
		return;

	movedDir *= moveSpeed * deltaTime;

	glm::vec3 camPos = camera->getPosition();
	Camera::Cam::LookAngles camAngles = camera->getLookAngles();

	camPos = camPos + movedDir.x * camAngles.right + movedDir.y * camAngles.up + movedDir.z * camAngles.forward;

	camera->setPosition(camPos);
}

void Camera::CamController::updateRotation(const glm::vec2& mouseDelta, float deltaTime) {
	if (mouseDelta == glm::vec2(0))
		return;

	Camera::Cam::LookAngles camAngles = camera->getLookAngles();
	glm::vec3 facing = camAngles.forward;

	facing = glm::rotate(facing, -mouseDelta.x * rotSpeed * deltaTime, camAngles.up);
	facing = glm::rotate(facing, mouseDelta.y * rotSpeed * deltaTime, camAngles.right);

	camera->setFacing(facing);
}