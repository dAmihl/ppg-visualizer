#pragma once

#include <GLFW/glfw3.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "PuzzGen.h"
#include <math.h>
#include <algorithm>

namespace PPGViz
{

	constexpr float NodeSize = 30.f;
	constexpr float NodeSpacing = NodeSize * 2 + 30.f;

	class WindowContext
	{

	public:
		WindowContext();
		~WindowContext();

		void update();

		void render();

		bool shouldClose() const;

		void drawPuzzle(PPG::Puzzle& P);
	private:
		GLFWwindow* window;
		ImVec4 clear_color;
	};

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

}

