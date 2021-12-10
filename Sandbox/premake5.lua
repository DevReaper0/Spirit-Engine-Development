project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/SpiritEngine/vendor/spdlog/include",
		"%{wks.location}/SpiritEngine/src",
		"%{wks.location}/SpiritEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"SpiritEngine"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SPIRIT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SPIRIT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SPIRIT_DIST"
		runtime "Release"
		optimize "on"