#pragma once

#include "Cam.h"

namespace Camera {
	class PerspectiveCam : public Cam {
	public:
		PerspectiveCam(float fovY, float near, float far, float width, float height);

		const float getFov();
		const float getFov() const;
		void setFov(float fovY);

	protected:
		void calculateRayCache() override;
		void calculateProjection() override;
		void calculateView() override;

	private:
		float fov;
	};
}