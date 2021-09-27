#include "Yaml2Puzzle.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "WindowContext.h"

int main(int, char**)
{
	PPGViz::WindowContext w;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	Yaml2Puzzle y2p;
	auto P = y2p.generatePuzzleByFile("resources/universe1.yaml");

	// Main loop
	while (!w.shouldClose())
	{
		w.update();

		{
			ImGui::Begin("Puzzle Text");
			ImGui::Text(P->getExtendedTextualRepresentation().c_str());
			ImGui::End();
		}

		{
			ImGui::Begin("Puzzle Events");
			for (auto& e : P->getContext().getEvents())
			{
				auto btnText = (e->getRelatedObject().getObjectName() + ":" + e->getEventName());
				if (ImGui::Button(btnText.c_str()))
				{
					P->handleEvent(*e);
				}
			}
			ImGui::End();
		}

		w.render();
	}

	return 0;
}