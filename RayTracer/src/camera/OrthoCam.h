#pragma once

#include "Cam.h"

namespace Camera {
	class OrthoCam : public Cam {
	public:
		OrthoCam(float near, float far, float width, float height);
	protected:
		void calculateRayCache() override;
		void calculateProjection() override;
		void calculateView() override;
	};
}