#pragma once

#include "Cam.h"

#include "OrthoCam.h"
#include "PerspectiveCam.h"

#include <type_traits>

namespace Camera {
	class CamController {
	public:
		CamController();
		CamController(float moveSpeed, float rotSpeed);
		~CamController();

		template<typename T, typename... Args> requires (std::is_base_of<Cam, T>::value)
		const Cam* setCamera(Args&&... args) {
			if (camera)
				delete camera;
			camera = new T(std::forward<Args>(args)...);
			hasFov = dynamic_cast<const PerspectiveCam*>(camera) != nullptr;
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
		void setFacing(const glm::vec3& facing);

		void setSize(float width, float height);
		void setSize(float width, float height, float near, float far);
		void setPlanes(float near, float far);

		const float getFov();
		void setFov(float fovY);

	private:
		void updatePosition(float deltaTime);
		void updateRotation(const glm::vec2& mouseDelta, float deltaTime);

	private:
		float moveSpeed;
		float rotSpeed;

		bool hasFov;
		Cam* camera;

		glm::vec2 prevMousePos;
	};
}