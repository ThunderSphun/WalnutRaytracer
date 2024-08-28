#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"
#include "Walnut/Input/Input.h"

#include "Renderer.h"
#include "Storage.h"
#include "camera/PerspectiveCam.h"
#include "camera/OrthoCam.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace constants {
	namespace settings {
		bool g_showSettings = true;
		bool g_showViewport = true;
		bool g_showDemo = false;
	}
}

extern glm::vec3 g_lightPos;

Walnut::Application* g_application;

class RaytraceLayer : public Walnut::Layer {
public:
	RaytraceLayer() : viewportSize(0, 0), camera(new Camera::PerspectiveCam(45, 0.001f, 100, viewportSize.x, viewportSize.y)), frameTime(0) {
		scene.spheres.emplace_back() = {glm::vec3(0, 0, -10), 0.5, glm::vec4(1, 0.5, 0.25, 1)};
		scene.spheres.emplace_back() = {glm::vec3(-0.5, -0.5, 5.5), 1, glm::vec4(0.25, 0, 1, 1)};
		camera->setPosition({0, 0, 6});
		camera->setFacing({0, 0, -1});
	}

	~RaytraceLayer() {
		delete camera;
	}

	virtual void OnUIRender() override {
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Escape))
			g_application->Close();

		static bool shouldRender = true;

		if (constants::settings::g_showSettings) {
			ImGui::Begin("settings", &constants::settings::g_showSettings);
			ImGui::Text("Last render: %.3fms", frameTime);
			ImGui::SameLine();
			ImGui::Checkbox("render", &shouldRender);

			glm::vec3 camPos = camera->getPosition();
			glm::vec3 camLookDir = camera->getFacing();

			if (ImGui::DragFloat3("camera position", glm::value_ptr(camPos)))
				camera->setPosition(camPos);
				
			if (ImGui::DragFloat3("camera looking direction", glm::value_ptr(camLookDir), 0.01f, 0, 1))
				camera->setFacing(camLookDir);

			ImGui::Separator();

			if (ImGui::Button("add sphere"))
				scene.spheres.emplace_back();

			ImGui::Separator();

			for (size_t i = 0; i < scene.spheres.size(); i++) {
				Storage::Sphere& current = scene.spheres[i];

				ImGui::PushID((int) i);

				ImGui::DragFloat3("sphere pos", glm::value_ptr(current.pos), 0.01f, -20, 20);
				ImGui::DragFloat("radius", &current.radius, 0.01f, 0, 10);
				ImGui::ColorEdit3("sphere color", glm::value_ptr(current.color), ImGuiColorEditFlags_Float);

				ImGui::Separator();
				ImGui::PopID();
			}

			ImGui::DragFloat3("light pos", glm::value_ptr(g_lightPos), 0.01f, -2, 2);
			ImGui::Separator();

			ImGui::End();
		}

		if (constants::settings::g_showViewport) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Viewport", &constants::settings::g_showViewport);

			viewportSize = ImGui::GetContentRegionAvail();

			auto image = renderer.GetFinalImage();
			if (image)
				ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(), (float)image->GetHeight()}, {0, 1}, {1, 0});

			ImGui::End();
			ImGui::PopStyleVar();
		}

		if (constants::settings::g_showDemo)
			ImGui::ShowDemoWindow(&constants::settings::g_showDemo);

		Walnut::Timer timer;
		if (shouldRender)
			render();
		frameTime = timer.ElapsedMillis();
	}

	void render() {
		camera->setSize(viewportSize.x, viewportSize.y);
		renderer.onResize({viewportSize.x, viewportSize.y});
		renderer.render(scene, camera);
	}

private:
	ImVec2 viewportSize;

	Renderer renderer;
	Storage::Scene scene;
	Camera::Cam* camera;

	float frameTime;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Raytracer";

	g_application = new Walnut::Application(spec);

	g_application->PushLayer<RaytraceLayer>();
	g_application->SetMenubarCallback([]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit"))
				g_application->Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("view")) {
			if (ImGui::MenuItem("viewport", nullptr, false, !constants::settings::g_showViewport))
				constants::settings::g_showViewport = true;
			if (ImGui::MenuItem("settings", nullptr, false, !constants::settings::g_showSettings))
				constants::settings::g_showSettings = true;
			ImGui::EndMenu();
		}

#ifndef WL_DIST
		if (ImGui::BeginMenu("debug")) {
			if (ImGui::MenuItem("demo", nullptr, false, !constants::settings::g_showDemo))
				constants::settings::g_showDemo = true;
			ImGui::EndMenu();
		}
#endif
	});

	return g_application;
}