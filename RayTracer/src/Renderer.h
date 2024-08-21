#pragma once

#include <Walnut/Image.h>
#include <glm/glm.hpp>
#include <memory>

class Renderer {
public:
	Renderer() = default;

	void onResize(glm::ivec2 size);
	void render();

	std::shared_ptr<Walnut::Image> GetFinalImage() const;

private:
	glm::vec4 pixel(glm::vec2 coord) const;

private:
	std::shared_ptr<Walnut::Image> resultImage;
	std::shared_ptr<glm::vec4[]> pixelData;
};