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
	include "SpiritEngine/vendor/GLFW"
	include "SpiritEngine/vendor/Glad"
	include "SpiritEngine/vendor/imgui"
	include "SpiritEngine/vendor/yaml-cpp"
group ""

include "SpiritEngine"
include "Sandbox"
include "SpiritEditor"
