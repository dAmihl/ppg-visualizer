workspace "ppg-visualizer"
	language "C++"
	cppdialect "C++14"
	configurations { "Debug", "Release"}
	platforms { "x64" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"



project "ppg-visualizer"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	links {"PPG", "PPG-YAML", "yaml-cpp", "GLFW", "OpenGL32"}
	files {
		"src/**.h", 
		"src/**.cpp",
		"vendor/imgui/**.cpp",
		"vendor/imgui/**.h"
	}
	
	
	includedirs {
		"vendor/ppg-yaml/src", 
		"vendor/ppg-yaml/vendor/yaml-cpp/include", 
		"vendor/ppg-yaml/vendor/ppg-core/src",
		"vendor/glfw/include",
		"vendor/imgui"
	}
	
	filter "configurations:Debug"
		defines {"DEBUG"}
		symbols "On"
		
	filter "configurations:Release"
		defines {"NDEBUG"}
		symbols "On"

include "vendor/ppg-yaml/vendor/ppg-core/ppg-core.lua"
include "vendor/ppg-yaml/ppg-yaml.lua"
include "vendor/glfw"


