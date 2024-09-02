#pragma once

#include "Cam.h"

#include "OrthoCam.h"
#include "PerspectiveCam.h"

#include <type_traits>

typedef int CamControllerFlags;
enum CamControllerFlags_ {
	CamControllerFlags_none = 0,
	CamControllerFlags_camMoveWhenMousePressed		= 1 << 0,
	CamControllerFlags_camRotWhenMousePressed		= 1 << 1,
	CamControllerFlags_moveXZPlaneIgnoresY			= 1 << 2,
	CamControllerFlags_moveTowardsCameraFacing		= 1 << 3,
	CamControllerFlags_camControlNeedsMousePress	= CamControllerFlags_camMoveWhenMousePressed | CamControllerFlags_camRotWhenMousePressed,
};

namespace Camera {
	class CamController {
	public:
		CamController();
		CamController(float moveSpeed, float rotSpeed, CamControllerFlags flags = CamControllerFlags_camControlNeedsMousePress | CamControllerFlags_moveTowardsCameraFacing);
		~CamController();

		template<typename T, typename... Args> requires (std::is_base_of<Cam, T>::value)
		const Cam* setCamera(Args&&... args) {
			if (camera)
				delete camera;
			camera = new T(std::forward<Args>(args)...);
			camHasFov = dynamic_cast<const PerspectiveCam*>(camera) != nullptr;
			return camera;
		}

		const Cam* getCamera();

		void update(float elapsedTime);

		const float getMoveSpeed();
		void setMoveSpeed(float speed);

		const float getRotSpeed();
		void setRotSpeed(float speed);

		const glm::vec3& getPosition();
		void setPosition(const glm::vec3& pos);

		const glm::vec3& getFacing();
		const Cam::LookAngles& getLookAngles();
		void setFacing(const glm::vec3& facing);

		const glm::vec2& getSize();
		const glm::vec2& getPlanes();
		void setSize(float width, float height);
		void setSize(float width, float height, float near, float far);
		void setPlanes(float near, float far);

		const bool hasFov();
		const float getFov();
		void setFov(float fovY);

		const CamControllerFlags& getFlags();
		void setFlags(CamControllerFlags flags);

	private:
		void updatePosition(float deltaTime);
		void updateRotation(const glm::vec2& mouseDelta, float deltaTime);
		bool hasFlag(CamControllerFlags flag);
		static bool hasFlag(CamControllerFlags flag, CamControllerFlags checkAgainst);

	private:
		CamControllerFlags flags;

		float moveSpeed;
		float rotSpeed;

		bool camHasFov;
		Cam* camera;

		glm::vec2 prevMousePos;
	};
}