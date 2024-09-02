#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"
#include "Walnut/Input/Input.h"

#include "Renderer.h"
#include "Storage.h"
#include "camera/CamController.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <iostream>

namespace constants {
	namespace settings {
		bool g_showSettings = true;
		bool g_showViewport = true;
		bool g_showDemo = false;
		bool g_showStyleEditor = false;
	}
}

extern glm::vec3 g_lightPos;

class RaytraceLayer : public Walnut::Layer {
public:
	RaytraceLayer() : viewportSize(0, 0), camera(1.0f, 0.2f, CamControllerFlags_camRotWhenMousePressed | CamControllerFlags_moveXZPlaneIgnoresY), frameTime(0) {
		scene.spheres.emplace_back() = {glm::vec3(0, 0, -2), 0.5, glm::vec4(1, 0.5, 0.25, 1)};
		scene.spheres.emplace_back() = {glm::vec3(-0.5, -0.5, -3), 1, glm::vec4(0.25, 0, 1, 1)};

		camera.setCamera<Camera::PerspectiveCam>(45.0f, 0.001f, 100.0f, viewportSize.x, viewportSize.y);
		camera.setPosition({0, 0, 0});
		camera.setFacing({0, 0, -1});
	}

	virtual void OnUpdate(float deltaTime) override {
		camera.update(deltaTime);
	}

	virtual void OnUIRender() override {
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Escape))
			Walnut::Application::Get().Close();

		static bool shouldRender = true;

		if (constants::settings::g_showSettings) {
			if (ImGui::Begin("settings", &constants::settings::g_showSettings)) {
				ImGui::Text("Last render: %.3fms", frameTime);
				ImGui::SameLine();
				ImGui::Checkbox("render", &shouldRender);

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Camera")) {
					ImGui::PushID("Camera");
					renderCameraUI();
					ImGui::PopID();
				}

				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Scene")) {
					ImGui::PushID("Scene");
					renderSceneUI();
					ImGui::PopID();
				}

			}
			ImGui::End();
		}

		if (constants::settings::g_showViewport) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			if (ImGui::Begin("Viewport", &constants::settings::g_showViewport)) {

				viewportSize = ImGui::GetContentRegionAvail();

				std::shared_ptr<Walnut::Image> image = renderer.GetFinalImage();
				if (image)
					ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, { 0, 1 }, { 1, 0 });

			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

		if (constants::settings::g_showDemo)
			ImGui::ShowDemoWindow(&constants::settings::g_showDemo);
		if (constants::settings::g_showStyleEditor) {
			if (ImGui::Begin("Dear ImGui Style Editor", &constants::settings::g_showStyleEditor))
				ImGui::ShowStyleEditor();
			ImGui::End();
		}

		Walnut::Timer timer;
		if (shouldRender)
			render();
		frameTime = timer.ElapsedMillis();

#ifdef WL_DEBUG
		shouldRender = false;
#endif
	}

	void render() {
		camera.setSize(viewportSize.x, viewportSize.y);
		renderer.onResize({viewportSize.x, viewportSize.y});
		renderer.render(scene, camera.getCamera());
	}

private:
	void renderCameraUI() {
		const float input3Width = -ImGui::GetWindowWidth() * 0.7f;

		{
			static int currentCam = 0;
			if (ImGui::Combo("selected camera", &currentCam, "perspective\0orthographic\0reset\0\0")) {
				glm::vec3 oldPos = camera.getPosition();
				glm::vec3 oldFacing = camera.getFacing();

				switch (currentCam) {
				default:
					std::cerr << "selected camera " << currentCam << " which does not exist, changing to default camera" << std::endl;
				case 2: // reset
					oldPos = {0, 0, 0};
					oldFacing = {0, 0, -1};
					currentCam = 0;
				case 0: // perspective
					camera.setCamera<Camera::PerspectiveCam>(45.0f, 0.001f, 100.0f, viewportSize.x, viewportSize.y);
					break;
				case 1: // ortho
					camera.setCamera<Camera::OrthoCam>(0.001f, 100.0f, viewportSize.x, viewportSize.y);
					break;
				}

				camera.setPosition(oldPos);
				camera.setFacing(oldFacing);
			}
			ImGui::Separator();
		}

		CamControllerFlags cameraFlags = camera.getFlags();

		{
			glm::vec3 camPos = camera.getPosition();
			float camMoveSpeed = camera.getMoveSpeed();
			static bool camMousePressedMoveNeeded = true;
			ImGui::BeginDisabled();
			ImGui::InputFloat3("pos", glm::value_ptr(camPos));
			ImGui::EndDisabled();

			ImGui::SetNextItemWidth(input3Width);
			if (ImGui::SliderFloat("move speed", &camMoveSpeed, 0, 100))
				camera.setMoveSpeed(camMoveSpeed);
			ImGui::SameLine();
			ImGui::CheckboxFlags("require mouse click##move", &cameraFlags, CamControllerFlags_camMoveWhenMousePressed);

			if (ImGui::CheckboxFlags("move towards camera forward", &cameraFlags, CamControllerFlags_moveTowardsCameraFacing))
				cameraFlags &= ~CamControllerFlags_moveXZPlaneIgnoresY;
			if (ImGui::CheckboxFlags("move on xz plane", &cameraFlags, CamControllerFlags_moveXZPlaneIgnoresY))
				cameraFlags &= ~CamControllerFlags_moveTowardsCameraFacing;
		}
		
		{
			Camera::Cam::LookAngles camAngles = camera.getLookAngles();
			float camRotSpeed = camera.getRotSpeed();
			static bool camMousePressedRotNeeded = true;

			ImGui::BeginDisabled();
			ImGui::Text("look angles");
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::InputFloat3("forward", glm::value_ptr(camAngles.forward));
			ImGui::InputFloat3("up", glm::value_ptr(camAngles.up));
			ImGui::InputFloat3("right", glm::value_ptr(camAngles.right));
			ImGui::EndDisabled();

			ImGui::SetNextItemWidth(input3Width);
			if (ImGui::SliderFloat("rotate speed", &camRotSpeed, 0, 100))
				camera.setRotSpeed(camRotSpeed);
			ImGui::SameLine();
			ImGui::CheckboxFlags("require mouse click##rotate", &cameraFlags, CamControllerFlags_camRotWhenMousePressed);

			ImVec2 buttonSize = {ImGui::CalcTextSize("+X").x * 2, 0};
			ImGui::BeginGroup();
			if (ImGui::Button("+X", buttonSize))
				camera.setFacing({1, 0, 0});
			if (ImGui::Button("-X", buttonSize))
				camera.setFacing({-1, 0, 0});
			ImGui::EndGroup();

			ImGui::SameLine();

			ImGui::BeginGroup();
			if (ImGui::Button("+Y", buttonSize))
				camera.setFacing({0, 1, 0});
			if (ImGui::Button("-Y", buttonSize))
				camera.setFacing({0, -1, 0});
			ImGui::EndGroup();

			ImGui::SameLine();

			ImGui::BeginGroup();
			if (ImGui::Button("-Z", buttonSize))
				camera.setFacing({0, 0, -1});
			if (ImGui::Button("+Z", buttonSize))
				camera.setFacing({0, 0, 1});
			ImGui::EndGroup();
		}

		camera.setFlags(cameraFlags);
#ifdef WL_DEBUG
		{
			glm::mat4 projection = camera.getCamera()->getProjection();
			glm::mat4 inverseProjection = camera.getCamera()->getInverseProjection();
			glm::mat4 view = camera.getCamera()->getView();
			glm::mat4 inverseView = camera.getCamera()->getInverseView();

			ImGui::BeginDisabled();
			ImGui::Text("projection");
			ImGui::SameLine();
			ImGui::Separator();
			for (int i = 0; i < 4; i++) 
				ImGui::InputFloat4("", glm::value_ptr(projection[i]));
			
			ImGui::Text("inverse projection");
			ImGui::SameLine();
			ImGui::Separator();
			for (int i = 0; i < 4; i++) 
				ImGui::InputFloat4("", glm::value_ptr(projection[i]));
			
			ImGui::Text("view");
			ImGui::SameLine();
			ImGui::Separator();
			for (int i = 0; i < 4; i++) 
				ImGui::InputFloat4("", glm::value_ptr(projection[i]));
			
			ImGui::Text("inverse view");
			ImGui::SameLine();
			ImGui::Separator();
			for (int i = 0; i < 4; i++) 
				ImGui::InputFloat4("", glm::value_ptr(projection[i]));
			
			ImGui::EndDisabled();
		}
#endif

		{
			glm::vec2 camSize = camera.getSize();
			glm::vec2 camPlanes = camera.getPlanes();

			ImGui::BeginDisabled();
			ImGui::Text("frustum");
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::InputFloat2("size", glm::value_ptr(camSize));
			ImGui::InputFloat2("planes", glm::value_ptr(camPlanes));
			ImGui::EndDisabled();
		}

		{
			float camFov = glm::radians(camera.getFov());

			ImGui::Separator();
			if (ImGui::SliderAngle("fov", &camFov, 0, 180))
				camera.setFov(glm::degrees(camFov));
		}
	}
	
	void renderSceneUI() {
		ImGui::Text("objects");
		ImGui::SameLine();
		ImGui::Separator();
		if (ImGui::Button("add sphere")) {
			Storage::Sphere& randomSphere = scene.spheres.emplace_back();
			randomSphere.pos = Walnut::Random::Vec3(-1, 1);
			randomSphere.radius = glm::abs(Walnut::Random::Float() * 2);
			randomSphere.albedo = glm::vec4(glm::rgbColor(glm::vec3(Walnut::Random::UInt(0, 360), 1, 1)), 1);
		}

		ImGui::Indent();

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
		if (ImGui::BeginChild("object list", {0, ImGui::GetFrameHeight() * 15}, true)) {
			for (size_t i = 0; i < scene.spheres.size(); i++) {
				ImGui::PushID((int)i);

				if (i > 0)
					ImGui::Separator();

				renderSingleSphereUI(i);

				ImGui::PopID();
			}
		}
		ImGui::EndChild();

		ImGui::PopStyleVar();
		ImGui::Unindent();

		ImGui::Separator();
		ImGui::DragFloat3("light pos", glm::value_ptr(g_lightPos), 0.01f, -2, 2);
		ImGui::Separator();
	}

	void renderSingleSphereUI(size_t i) {
		Storage::Sphere& current = scene.spheres[i];

		ImGui::DragFloat3("position", glm::value_ptr(current.pos), 0.01f, -20, 20);
		ImGui::DragFloat("radius", &current.radius, 0.01f, 0, 10);
		ImGui::ColorEdit3("albedo", glm::value_ptr(current.albedo), ImGuiColorEditFlags_Float);

		if (ImGui::Button("remove"))
			scene.spheres.erase(scene.spheres.begin() + i);
	}

private:
	ImVec2 viewportSize;

	Renderer renderer;
	Storage::Scene scene;
	Camera::CamController camera;

	float frameTime;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Raytracer";

	Application* application = new Walnut::Application(spec);

	application->PushLayer<RaytraceLayer>();
	application->SetMenubarCallback([application]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit"))
				application->Close();
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
			if (ImGui::MenuItem("style editor", nullptr, false, !constants::settings::g_showStyleEditor))
				constants::settings::g_showStyleEditor = true;
			ImGui::EndMenu();
		}
#endif
	});

	return application;
}