include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "SpiritEngine"
	architecture "x86_64"
	startproject "SpiritEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "SpiritEngine/vendor/Box2D"
	include "SpiritEngine/vendor/GLFW"
	include "SpiritEngine/vendor/Glad"
	include "SpiritEngine/vendor/imgui"
	include "SpiritEngine/vendor/yaml-cpp"
group ""

include "SpiritEngine"
include "Sandbox"
include "SpiritEditor"





--include "Dependencies.lua"
--
--workspace "SpiritEngine"
--	architecture "x86_64"
--	startproject "SpiritEditor"
--
--	configurations
--	{
--		"Debug",
--		"Release",
--		"Dist"
--	}
--	
--	flags
--	{
--		"MultiProcessorCompile"
--	}
--
--outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
--
---- Include directories relative to root folder (solution directory)
--IncludeDir = {}
--IncludeDir["GLFW"] = "SpiritEngine/vendor/GLFW/include"
--IncludeDir["Glad"] = "SpiritEngine/vendor/Glad/include"
--IncludeDir["ImGui"] = "SpiritEngine/vendor/imgui"
--IncludeDir["glm"] = "SpiritEngine/vendor/glm"
--IncludeDir["stb_image"] = "SpiritEngine/vendor/stb_image"
--IncludeDir["SFML"] = "SpiritEngine/vendor/SFML-2.5.1/include"
--IncludeDir["entt"] = "SpiritEngine/vendor/entt/include"
--IncludeDir["yaml_cpp"] = "%{wks.location}/SpiritEngine/vendor/yaml-cpp/include"
--IncludeDir["ImGuizmo"] = "SpiritEngine/vendor/ImGuizmo"
----IncludeDir["rapidyaml"] = "%{wks.location}/SpiritEngine/vendor/rapidyaml/src"
--IncludeDir["shaderc"] = "%{wks.location}/SpiritEngine/vendor/shaderc/include"
--IncludeDir["SPIRV_Cross"] = "%{wks.location}/SpiritEngine/vendor/SPIRV-Cross"
--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
--
--group "Dependencies"
--	include "SpiritEngine/vendor/GLFW"
--	include "SpiritEngine/vendor/Glad"
--	include "SpiritEngine/vendor/imgui"
--	include "SpiritEngine/vendor/OpenAL-Soft"
--	include "SpiritEngine/vendor/libogg"
--	include "SpiritEngine/vendor/Vorbis"
--	include "SpiritEngine/vendor/yaml-cpp"
--	--include "SpiritEngine/vendor/rapidyaml"
--	--include "SpiritEngine/vendor/SOIL"
--	--include "SpiritEngine/vendor/AudioFile"
--group ""
--
--project "SpiritEngine"
--	location "SpiritEngine"
--	kind "StaticLib"
--	language "C++"
--	cppdialect "C++17"
--	staticruntime "off"
--
--	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
--
--	pchheader "spiritpch.h"
--	pchsource "SpiritEngine/src/spiritpch.cpp"
--
--	files
--	{
--		"%{prj.name}/src/**.h",
--		"%{prj.name}/src/**.cpp",
--		"%{prj.name}/vendor/stb_image/**.h",
--		"%{prj.name}/vendor/stb_image/**.cpp",
--		"%{prj.name}/vendor/glm/glm/**.hpp",
--		"%{prj.name}/vendor/glm/glm/**.inl",
--		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
--		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
--		"%{prj.name}/app.rc"
--	}
--
--	defines
--	{
--		"_CRT_SECURE_NO_WARNINGS",
--		"GLFW_INCLUDE_NONE",
--		"AL_LIBTYPE_STATIC"
--	}
--
--	includedirs
--	{
--		"%{prj.name}/src",
--		"%{prj.name}/vendor/spdlog/include",
--		"%{IncludeDir.GLFW}",
--		"%{IncludeDir.Glad}",
--		"%{IncludeDir.ImGui}",
--		"%{IncludeDir.glm}",
--		"%{IncludeDir.stb_image}",
--		"%{IncludeDir.entt}",
--		"%{IncludeDir.yaml_cpp}",
--		"%{IncludeDir.ImGuizmo}",
--		--"%{IncludeDir.rapidyaml}",
--		"%{IncludeDir.VulkanSDK}",
--		"SpiritEngine/vendor/OpenAL-Soft/include",
--		"SpiritEngine/vendor/OpenAL-Soft/src",
--		"SpiritEngine/vendor/OpenAL-Soft/src/common",
--		"SpiritEngine/vendor/libogg/include",
--		"SpiritEngine/vendor/Vorbis/include",
--		"SpiritEngine/vendor/minimp3",
--		"SpiritEngine/vendor/SOIL/incs",
--		"SpiritEngine/vendor/SOIL/srcs",
--		--"SpiritEngine/vendor/AudioFile"
--	}
--
--	links 
--	{ 
--		"GLFW",
--		"Glad",
--		"ImGui",
--		"opengl32.lib",
--		"OpenAL-Soft",
--		"Vorbis",
--		"yaml-cpp",
--		--"rapidyaml"--,
--		--"SOIL"
--		--"AudioFile"
--	}
--
--	filter "files:vendor/ImGuizmo/**.cpp"
--	flags { "NoPCH" }
--
--	filter "system:linux"
--		systemversion "latest"
--
--		defines
--		{
--		}
--	
--		filter "system:windows"
--		systemversion "latest"
--
--		defines
--		{
--		}
--
--	filter "configurations:Debug"
--		defines "SPIRIT_DEBUG"
--		runtime "Debug"
--		symbols "on"
--
--		links
--		{
--			"%{Library.ShaderC_Debug}",
--			"%{Library.SPIRV_Cross_Debug}",
--			"%{Library.SPIRV_Cross_GLSL_Debug}"
--		}
--
--	filter "configurations:Release"
--		defines "SPIRIT_RELEASE"
--		runtime "Release"
--		optimize "on"
--
--		links
--		{
--			"%{Library.ShaderC_Release}",
--			"%{Library.SPIRV_Cross_Release}",
--			"%{Library.SPIRV_Cross_GLSL_Release}"
--		}
--
--	filter "configurations:Dist"
--		defines "SPIRIT_DIST"
--		runtime "Release"
--		optimize "on"
--
--		links
--		{
--			"%{Library.ShaderC_Release}",
--			"%{Library.SPIRV_Cross_Release}",
--			"%{Library.SPIRV_Cross_GLSL_Release}"
--		}
--
--project "Sandbox"
--	location "Sandbox"
--	kind "ConsoleApp"
--	language "C++"
--	cppdialect "C++17"
--	staticruntime "on"
--
--	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
--
--	files
--	{
--		"%{prj.name}/src/**.h",
--		"%{prj.name}/src/**.cpp",
--		"%{prj.name}/app.rc"
--	}
--
--	includedirs
--	{
--		"%{prj.name}/src",
--		"%{prj.name}/vendor/spdlog/include",
--		"%{IncludeDir.GLFW}",
--		"%{IncludeDir.Glad}",
--		"%{IncludeDir.ImGui}",
--		"%{IncludeDir.glm}",
--		"%{IncludeDir.stb_image}",
--		"%{IncludeDir.entt}",
--		"%{IncludeDir.yaml_cpp}",
--		"%{IncludeDir.ImGuizmo}",
--		--"%{IncludeDir.rapidyaml}",
--		"SpiritEngine/vendor/OpenAL-Soft/include",
--		"SpiritEngine/vendor/OpenAL-Soft/src",
--		"SpiritEngine/vendor/OpenAL-Soft/src/common",
--		"SpiritEngine/vendor/libogg/include",
--		"SpiritEngine/vendor/Vorbis/include",
--		"SpiritEngine/vendor/minimp3",
--		"SpiritEngine/vendor/SOIL/incs",
--		"SpiritEngine/vendor/SOIL/srcs"
--		--"SpiritEngine/vendor/AudioFile"
--	}
--
--	links
--	{
--		"SpiritEngine"
--	}
--
--	filter "system:linux"
--		systemversion "latest"
--	
--	filter "system:windows"
--		systemversion "latest"
--		
--	filter "configurations:Debug"
--		defines "SPIRIT_DEBUG"
--		runtime "Debug"
--		symbols "on"
--
--	filter "configurations:Release"
--		defines "SPIRIT_RELEASE"
--		runtime "Release"
--		optimize "on"
--
--	filter "configurations:Dist"
--		defines "SPIRIT_DIST"
--		runtime "Release"
--		optimize "on"
--
--project "SpiritEditor"
--	location "SpiritEditor"
--	kind "ConsoleApp"
--	language "C++"
--	cppdialect "C++17"
--	staticruntime "on"
--
--	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
--	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
--
--	files
--	{
--		"%{prj.name}/src/**.h",
--		"%{prj.name}/src/**.cpp",
--		"%{prj.name}/app.rc"
--	}
--
--	includedirs
--	{
--		"SpiritEngine/vendor/spdlog/include",
--		"SpiritEngine/src",
--		"SpiritEngine/vendor",
--		"%{IncludeDir.glm}",
--		"%{IncludeDir.ImGuizmo}",
--		"%{IncludeDir.entt}"
--	}
--
--	links
--	{
--		"SpiritEngine"
--	}
--
--	filter "system:windows"
--		systemversion "latest"
--		
--	filter "configurations:Debug"
--		defines "SPIRIT_DEBUG"
--		runtime "Debug"
--		symbols "on"
--
--	filter "configurations:Release"
--		defines "SPIRIT_RELEASE"
--		runtime "Release"
--		optimize "on"
--
--	filter "configurations:Dist"
--		defines "SPIRIT_DIST"
--		runtime "Release"
--		optimize "on"
--