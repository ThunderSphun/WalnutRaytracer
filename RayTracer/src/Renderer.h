#pragma once

#include <Walnut/Image.h>
#include "Storage.h"
#include <glm/glm.hpp>
#include <memory>

class Renderer {
public:
	Renderer() = default;

	void onResize(glm::ivec2 size);
	void render(const Storage::Scene& scene);

	std::shared_ptr<Walnut::Image> GetFinalImage() const;

private:
	glm::vec4 traceRay(const Storage::Ray& ray) const;

private:
	std::shared_ptr<Walnut::Image> resultImage;
	std::shared_ptr<glm::vec4[]> pixelData;

	const Storage::Scene* currentScene = nullptr;
};