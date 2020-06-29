workspace "SpiritEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "SpiritEngine"
	location "SpiritEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"SPIRIT_PLATFORM_WINDOWS",
			"SPIRIT_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "SPIRIT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "SPIRIT_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SPIRIT_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"SpiritEngine/vendor/spdlog/include",
		"SpiritEngine/src"
	}

	links
	{
		"SpiritEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"SPIRIT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SPIRIT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "SPIRIT_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SPIRIT_DIST"
		optimize "On"