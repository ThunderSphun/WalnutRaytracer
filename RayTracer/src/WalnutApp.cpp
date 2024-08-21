#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Walnut/Random.h"
#include "Walnut/Input/Input.h"

#include "Renderer.h"

namespace settings {
	bool g_showSettings = true;
	bool g_showViewport = true;
	bool g_showDemo = false;
}

Walnut::Application* g_application;

class RaytraceLayer : public Walnut::Layer {
public:
	RaytraceLayer() {

	}

	virtual void OnUIRender() override {
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Escape))
			g_application->Close();

		static bool shouldRender = true;

		if (settings::g_showSettings) {
			ImGui::Begin("settings", &settings::g_showSettings);
			ImGui::Text("Last render: %.3fms", lastRenderTime);
			ImGui::SameLine();
			ImGui::Checkbox("render", &shouldRender);
			ImGui::End();
		}

		if (settings::g_showViewport) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Viewport", &settings::g_showViewport);

			viewportSize = ImGui::GetContentRegionAvail();

			auto image = renderer.GetFinalImage();
			if (image)
				ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(), (float)image->GetHeight()});

			ImGui::End();
			ImGui::PopStyleVar();
		}

		if (settings::g_showDemo)
			ImGui::ShowDemoWindow(&settings::g_showDemo);

		Walnut::Timer timer;
		if (shouldRender)
			render();
		lastRenderTime = timer.ElapsedMillis();
	}

	void render() {
		renderer.onResize({viewportSize.x, viewportSize.y});
		renderer.render();
	}

private:
	Renderer renderer;

	ImVec2 viewportSize = {0, 0};
	float lastRenderTime = 0;
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
			if (ImGui::MenuItem("viewport", nullptr, false, !settings::g_showViewport))
				settings::g_showViewport = true;
			if (ImGui::MenuItem("settings", nullptr, false, !settings::g_showSettings))
				settings::g_showSettings = true;
			ImGui::EndMenu();
		}

#ifndef WL_DIST
		if (ImGui::BeginMenu("debug")) {
			if (ImGui::MenuItem("demo", nullptr, false, !settings::g_showDemo))
				settings::g_showDemo = true;
			ImGui::EndMenu();
		}
#endif
	});

	return g_application;
}