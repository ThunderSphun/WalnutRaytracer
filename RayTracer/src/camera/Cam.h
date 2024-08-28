#pragma once

#include "Storage.h"

#include <vector>

namespace Camera {
	class Cam {
	public:
		Cam(float near, float far, float width, float height);

		const std::vector<Storage::Ray>& getRays();
		const std::vector<Storage::Ray>& getRays() const;

		const glm::mat4& getProjection();
		const glm::mat4& getInverseProjection();
		const glm::mat4& getView();
		const glm::mat4& getInverseView();

		const glm::mat4& getProjection() const;
		const glm::mat4& getInverseProjection() const;
		const glm::mat4& getView() const;
		const glm::mat4& getInverseView() const;

		const glm::vec3& getPosition();
		const glm::vec3& getPosition() const;
		void setPosition(const glm::vec3& pos);

		const glm::vec3& getFacing();
		const glm::vec3& getFacing() const;
		void setFacing(const glm::vec3& facing);

		void setSize(float width, float height);
		void setSize(float width, float height, float near, float far);
		void setPlanes(float near, float far);

	protected:
		virtual void calculateRayCache() = 0;
		virtual void calculateProjection() = 0;
		virtual void calculateView() = 0;
	protected:
		std::vector<Storage::Ray> rayCache;

		glm::mat4 projection;
		glm::mat4 inverseProjection;
		glm::mat4 view;
		glm::mat4 inverseView;

		glm::vec3 position;
		glm::vec3 lookdir;

		glm::vec2 size;
		float nearPlane;
		float farPlane;
	};
}