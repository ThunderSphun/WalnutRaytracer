#include "Renderer.h"

#include "Constants.h"

namespace constants {
	namespace scene {
		extern glm::vec3 g_sphereColor = { 1, 0.5, 0.25 };
		extern glm::vec3 g_spherePos = { 0, 0, -1 };
		extern glm::vec3 g_lightPos = { -1, -1, 0.5 };
	}
}

void Renderer::onResize(glm::ivec2 size) {
	bool changed = false;

	if (!resultImage) {
		resultImage = std::make_shared<Walnut::Image>(size.x, size.y, Walnut::ImageFormat::RGBA32F);
		changed = true;
	}

	if (resultImage->GetWidth() != size.x || resultImage->GetHeight() != size.y) {
		resultImage->Resize(size.x, size.y);
		changed = true;
	}

	if (changed)
		pixelData = std::make_shared<glm::vec4[]>(resultImage->GetWidth() * resultImage->GetHeight());
}

void Renderer::render() {
	float ratio = resultImage->GetWidth() / (float) resultImage->GetHeight();
	for (uint32_t y = 0; y < resultImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < resultImage->GetWidth(); x++) {
			uint32_t index = x + y * resultImage->GetWidth();

			glm::vec2 uv = glm::vec2((float)x / resultImage->GetWidth(), (float) y / resultImage->GetHeight()) * 2.0f - 1.0f;

			uv.x *= ratio;

			glm::vec4 color = pixel(uv);
			color.a = 1;
			pixelData[index] = color;
		}
	}

	resultImage->SetData(pixelData.get());
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() const {
	return resultImage;
}

/// 
/// equation for a sphere is			`(x - Cx)^2 + (y - Cy)^2 + (z - Cz)^2 - r^2 = 0` where C is the coordinate of the circle center and r is the radius
/// equation for a line is				`y = ax + b`
/// equation for a quadratic line is	`y = ax^2 + bx + c`
/// equation for a ray is				`P = O + Dt` where P (point), O (origin) and D (direction) are vec3 and thus can be split up
///										`Px = Ox + Dx * t`
///										`Py = Oy + Dy * t`
///										`Pz = Oz + Dz * t`
/// quadratic formula is				`(-b +- sqrt(D))/(4*a)` where D (discriminant) is `b^2 - 4*a*c`
///																D indicates whether there are 0, 1 or 2 solutions with `D < 0` is 0, `D == 0` is 1 and `D > 0` is 2
/// 
/// We will simplify the equation for a sphere by ignoring the center coordinate, we can offset the ray origin in the opposite direction and get the same effect.
/// I believe the total calculations will be lower that way.
/// The new equation for a sphere will be	`x^2 + y^2 + z^2 - r^2 = 0`
/// 
/// if we substitute the ray equation into the sphere equation, we get the following equation
///			`(Ox + Dx * t)^2 + (Oy + Dy * t)^2 + (Oz + Dz * t)^2 - r^2 = 0`
///			`(Ox + Dx * t)(Ox + Dx * t) + (Oy + Dy * t)(Oy + Dy * t) + (Oz + Dz * t)(Oz + Dz * t) - r * r = 0`
/// lets just do the x component, the other components work the same way
///			`Ox * Ox + Ox * Dx * t + Dx * t * Ox + Dx * t * Dx * t`
///			`Ox^2 + 2 * Ox * Dx * t + Dx^2 * t^2`
///			`Ox^2 + 2t(Ox*Dx) + (Dx*t)^2`
/// which totals to
///			`Ox^2 + 2t(Ox*Dx) + (Dx*t)^2 + Oy^2 + 2t(Oy*Dy) + (Dy*t)^2 + Oz^2 + 2t(Oz*Dz) + (Dz*t)^2 - r^2 = 0`
/// 
/// this formula can be rewritten into a quadratic equation where t is the variable term
///			`(Dx + Dy + Dz)t^2 + (2(Ox*Dx + Oy*Dy + Oz*Dz))t + (Ox^2 + Oy^2 + Oz^2 - r^2) = 0`
/// 
/// 
/// Lets put it to the test now
/// lets say we have a sphere at (2, 1, 0) with radius 3, and a ray with origin (-2, -10, 0) and direction (4, 0, 0)
/// *(direction should be normalized for actual distance across the ray, but we just need to know if the ray hits the sphere)*
/// we need to offset the origin with the sphere location, so the origin moves to (-6, -11, 0)
/// we will substitute the actual values into the quadratic equation immediately, so we get the following:
///		`(4 + 0 + 0)t^2 + (2(-6*4 + -11*0 + 0*0))t + ((-6)^2 + (-11)^2 + 0^2 - 3^2) = 0`
///		`(4)t^2 + (2(-24))t + (36 + 121 - 9) = 0`
///		`(4)t^2 + (-48)t + (148) = 0`
/// by putting these values into the formula for the discriminant we get
///		`(-48)^2 - 4*4*148 = 2304 - 2368 = -64`
/// so we can conclude this ray misses the sphere
/// 
/// another test could be having the sphere at (2, 1, 4) and radius 3, with a ray with origin (-2, -10, 0) and direction (2, 6, 1)
/// *(again the direction should be normalized to get the distance from the sphere)*
/// again the origin will be offset with the sphere location, giving (-6, -11, -4)
/// lets substitute them again
///			`(2 + 6 + 1)t^2 + (2(-6*2 + -11*6 + -4*1))t + ((-6)^2 + (-11)^2 + (-4)^2 - 3^2) = 0`
///			`(9)t^2 + (2(-82))t + (36 + 121 + 16 - 9) = 0`
///			`(9)t^2 + (-164)t + (164) = 0`
/// by putting these values into the formula for the discriminant we get
///			`(-164)^2 - 4*9*164 = 26896 - 5904 = 20992`
/// so we can conclude that this ray hits the sphere twice
/// 
glm::vec4 Renderer::pixel(glm::vec2 coord) const {
	glm::vec3 origin = constants::scene::g_spherePos;

	glm::vec3 dir(coord, 1);

	float size = 0.5;

	float a = glm::dot(dir, dir);
	float b = 2.0f * glm::dot(origin, dir);
	float c = glm::dot(origin, origin) - size * size;

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);
	float distance = (-b - glm::sqrt(discriminant)) / (2 * a);

	glm::vec3 hitpos = (origin + dir * distance);
	glm::vec3 normal = hitpos / size; // simplified because of sphere

	glm::vec3 lightDir = glm::normalize(constants::scene::g_lightPos);

	float lightIntensity = glm::max(glm::dot(normal, -lightDir), 0.0f);

	glm::vec3 color = constants::scene::g_sphereColor;

	color *= lightIntensity;

	return glm::vec4(color, 1);
}
