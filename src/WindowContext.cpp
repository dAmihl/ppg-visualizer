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

	void drawNode(PPG::Ptr<PPG::Node>& n, ImVec2 pos, ImDrawList* draw_list)
	{
		float radius = NodeSize;
		ImU32 colInactive = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
		ImU32 colActive = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
		ImU32 colComplete = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));

		ImU32 col = colInactive;
		if (n->getCurrentNodeState() == PPG::ENodeState::ACTIVE)
		{
			col = colActive;
		}
		else if (n->getCurrentNodeState() == PPG::ENodeState::COMPLETED)
		{
			col = colComplete;
		}
		draw_list->AddCircle(pos, NodeSize, col);
		//draw_list->AddRect(pos, ImVec2{ pos.x + NodeSize * 2, pos.y + NodeSize * 2 }, col);
		PPG::Str desc = n->getRelatedObject().getObjectName() + " \n " + n->getRelatedObject().getCurrentState().getName();
		draw_list->AddText(ImVec2(pos.x - NodeSize/2, pos.y), col, desc.c_str());
		
	}

	void drawArrow(ImVec2 from, ImVec2 to, ImDrawList* draw_list)
	{
		IMGUI_API ImU32 colorBlue = ImGui::GetColorU32(IM_COL32(0, 0, 255, 255));
		IMGUI_API ImU32 colorWhite = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
		
		float dirX = to.x - from.x;
		float dirY = to.y - from.y;
		float len = dirX * dirX + dirY * dirY;
		ImVec2 arrowDir{ dirX / sqrt(len), dirY / sqrt(len) };
		ImVec2 headStart{ to.x - NodeSize * arrowDir.x, to.y - NodeSize * arrowDir.y };
		ImVec2 fromStart{ from.x + NodeSize * arrowDir.x, from.y + NodeSize * arrowDir.y };
		draw_list->AddLine(fromStart, headStart, colorWhite, 1.0f);
		draw_list->AddRectFilled(ImVec2{ headStart.x - 2.5f, headStart.y - 2.5f }, ImVec2{ headStart.x + 2.5f, headStart.y + 2.5f }, colorBlue);
	}


	void drawSubgraph(PPG::Puzzle& P, std::unordered_map<PPG::Ptr<PPG::Node>, ImVec2>& nodePosMap, PPG::Vec<PPG::Ptr<PPG::Node>>& fol, ImDrawList* draw_list, ImVec2 center, size_t col, size_t row, float width )
	{
		size_t num = fol.size();
		
		float sp = width / num;
		float ox = center.x - (((num - 1) / 2) * sp);

		auto& minima = P.getRelation().getMinima(fol);

		for (auto& f : fol)
		{
			ImVec2 pos;
			auto& entry = nodePosMap.find(f);

			if (entry == nodePosMap.end())
			{
				float x = ox + (col * sp);
				pos = ImVec2{x, center.y};
				
				drawNode(f, pos, draw_list);
				nodePosMap.emplace(f, pos);
			}
			else
			{
				pos = entry->second;
			}

			col++;

			auto& next = P.getRelation().getFollowingNodes(f);
			drawSubgraph(P, nodePosMap, next, draw_list, ImVec2{ pos.x + NodeSpacing + row, pos.y + NodeSpacing }, 0, row + 1, sp * (row +1));
		}
	}
	
	void WindowContext::drawPuzzle(PPG::Puzzle& P)
	{
		std::unordered_map<PPG::Ptr<PPG::Node>, ImVec2> nodePosMap;
		auto& nodes = P.getNodes();
		PPG::NodePairVec pairs = P.getRelation().getPairs();
		
		auto& minima = P.getRelation().getMinima(nodes);

		ImGui::Begin("Puzzle Nodes");
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		
		float ox = p0.x + 500.f;
		float oy = p0.y + 100.f;

		size_t col = 0;
		size_t row = 0;
		
		// Draw Nodes
		drawSubgraph(P, nodePosMap, minima, draw_list, ImVec2{ ox,  oy }, col, row, 1024.f);

		// Draw Arrows
		for (auto& p : pairs)
		{
			auto& entryFirst = nodePosMap.find(p.first);
			auto& entrySec = nodePosMap.find(p.second);

			if (entryFirst == std::end(nodePosMap) || entrySec == std::end(nodePosMap)) continue;

			drawArrow(entryFirst->second, entrySec->second, draw_list);
		}

		ImGui::End();
	}

}