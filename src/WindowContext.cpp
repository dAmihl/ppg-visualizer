#include "WindowContext.h"

namespace PPGViz
{

	WindowContext::WindowContext()
	{
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			throw;

		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		// Create window with graphics context
		window = glfwCreateWindow(1280, 720, "PPG-VISUALIZER", NULL, NULL);
		
		if (window == NULL)
			throw;

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	}

	WindowContext::~WindowContext()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void WindowContext::update()
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void WindowContext::render()
	{
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	bool WindowContext::shouldClose() const
	{
		return glfwWindowShouldClose(window);
	}

	void WindowContext::drawPuzzle(PPG::Puzzle& P)
	{
		std::unordered_map<PPG::Ptr<PPG::Node>, ImVec2> nodePosMap;
		float radius = 10.f;
		auto& pairs = P.getRelation().getPairs();
		ImGui::Begin("Puzzle Nodes");
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		int ox = p0.x + 50;
		int oy = p0.y + 50;
		int c = 0;
		for (auto& p : pairs)
		{
			ImVec2 posFirst;
			ImVec2 posSecond;
			auto& entryFirst = nodePosMap.find(p.first);
			auto& entrySecond = nodePosMap.find(p.second);
			
			if (entryFirst == nodePosMap.end())
			{
				posFirst = ImVec2(ox + (c / 3) * 100, oy + (c % 3) * 100);
				draw_list->AddCircle(posFirst, radius, ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)));
				c++;
				nodePosMap.emplace(p.first, posFirst);
			}
			else
			{
				posFirst = (*entryFirst).second;
			}

			if (entrySecond == nodePosMap.end())
			{
				posSecond = ImVec2(ox + (c / 3) * 100, oy + (c % 3) * 100);
				draw_list->AddCircle(posSecond, radius, ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)));
				c++;
				nodePosMap.emplace(p.second, posSecond);
			}
			else
			{
				posSecond = (*entrySecond).second;
			}

			draw_list->AddLine(posFirst, posSecond, ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 1.0f);
			
		}
		ImGui::End();
	}

}