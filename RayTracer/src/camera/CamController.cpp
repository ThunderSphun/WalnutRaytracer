#include "CamController.h"

#include "Walnut/Input/Input.h"
#include <glm/gtx/rotate_vector.hpp>

static constexpr glm::vec2 zero2(0);
static constexpr glm::vec3 zero3(0);
static constexpr Camera::Cam::LookAngles zeroLA = {zero3, zero3, zero3};

Camera::CamController::CamController() : CamController(1, 1) {}

Camera::CamController::CamController(float moveSpeed, float rotSpeed, CamControllerFlags flags) :
	moveSpeed(moveSpeed), rotSpeed(rotSpeed), camHasFov(false), camera(nullptr), prevMousePos(0), flags(flags) {
	setFlags(flags);
}

Camera::CamController::~CamController() {
	delete camera;
}

const Camera::Cam* Camera::CamController::getCamera() {
	return camera;
}

void Camera::CamController::update(float deltaTime) {
	glm::vec2 mousePos = Walnut::Input::GetMousePosition();
	glm::vec2 mouseDelta = (mousePos - prevMousePos);
	prevMousePos = mousePos;

	if (!camera)
		return;

	bool isClicking = Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right);

	if (hasFlag(CamControllerFlags_camMoveWhenMousePressed) && isClicking) {
		updatePosition(deltaTime);
	} else if (!hasFlag(CamControllerFlags_camMoveWhenMousePressed))
		updatePosition(deltaTime);

	if (hasFlag(CamControllerFlags_camRotWhenMousePressed) && isClicking) {
		Walnut::Input::SetCursorMode(Walnut::CursorMode::Locked);
		updateRotation(mouseDelta, deltaTime);
	} else if (!hasFlag(CamControllerFlags_camRotWhenMousePressed))
		updateRotation(mouseDelta, deltaTime);

	if (!isClicking)
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
	return camera ? camera->getPosition() : zero3;
}

void Camera::CamController::setPosition(const glm::vec3& pos) {
	if (camera)
		camera->setPosition(pos);
}

const glm::vec3& Camera::CamController::getFacing() {
	return camera ? camera->getFacing() : zero3;
}

const Camera::Cam::LookAngles& Camera::CamController::getLookAngles() {
	return camera ? camera->getLookAngles() : zeroLA;
}

void Camera::CamController::setFacing(const glm::vec3& facing) {
	if (camera)
		camera->setFacing(facing);
}

const glm::vec2& Camera::CamController::getSize() {
	return camera ? camera->getSize() : zero2;
}

const glm::vec2& Camera::CamController::getPlanes() {
	return camera ? camera->getPlanes() : zero2;
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

const bool Camera::CamController::hasFov() {
	return camera && camHasFov;
}

const float Camera::CamController::getFov() {
	return hasFov() ? static_cast<PerspectiveCam*>(camera)->getFov() : 0;
}

void Camera::CamController::setFov(float fovY) {
	if (hasFov())
		static_cast<PerspectiveCam*>(camera)->setFov(fovY);
}

const CamControllerFlags& Camera::CamController::getFlags() {
	return flags;
}

void Camera::CamController::setFlags(CamControllerFlags flags) {
	assert(!(hasFlag(CamControllerFlags_moveXZPlaneIgnoresY, flags)
		&& hasFlag(CamControllerFlags_camControlNeedsMousePress, flags)));
	this->flags = flags;
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

	if (hasFlag(CamControllerFlags_moveXZPlaneIgnoresY)) {
		if (glm::abs(camAngles.forward) == glm::vec3(0, 1, 0))
			return;

		camAngles.up = glm::vec3(0, 1, 0);

		camAngles.right.y = 0;
		camAngles.right = glm::normalize(camAngles.right);
	} else if (!hasFlag(CamControllerFlags_moveXZPlaneIgnoresY) && !hasFlag(CamControllerFlags_moveTowardsCameraFacing)) {
		if (glm::abs(camAngles.forward) == glm::vec3(0, 1, 0))
			return;

		if (glm::abs(camAngles.forward.x) >= glm::abs(camAngles.forward.z)) {
			camAngles.forward = glm::vec3((camAngles.forward.x > 0) ? 1 : -1, 0, 0);
			camAngles.right =   glm::vec3(0, 0, (camAngles.forward.x > 0) ? 1 : -1);
		} else {
			camAngles.forward = glm::vec3(0, 0, (camAngles.forward.z > 0) ? 1 : -1);
			camAngles.right =   glm::vec3((camAngles.forward.z > 0) ? -1 : 1, 0, 0);
		}

		camAngles.up = glm::vec3(0, -1, 0);
	}

	camPos += movedDir.x * camAngles.right + movedDir.y * camAngles.up + movedDir.z * camAngles.forward;

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

bool Camera::CamController::hasFlag(CamControllerFlags flag) {
	return hasFlag(flag, flags);
}

bool Camera::CamController::hasFlag(CamControllerFlags flag, CamControllerFlags checkAgainst) {
	return (checkAgainst & flag) == flag;
}
