OUTPUT_DIRECTORY = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

local root_directory = "../"

include(root_directory .. "dependencies.lua")

workspace "Sandbox"
	defines
	{
		"Grapple_BUILD_SHARED"
	}

	architecture "x86_64"

	configurations
	{
		"Debug",
		"Release",
		"Dist",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	project "Sandbox"
		kind "SharedLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		files
		{
			"Assets/**.h",
			"Assets/**.cpp",
		}

		includedirs
		{
			"src/",

			root_directory .. "GrappleScriptingCore/src/",
			root_directory .. "GrappleCommon/src/",
			root_directory .. "Grapple/vendor/spdlog/include/"
		}

		links
		{
			"GrappleCommon",
			"GrappleScriptingCore",
		}

		targetdir("%{wks.location}/bin/" .. OUTPUT_DIRECTORY .. "/%{prj.name}")
		objdir("%{wks.location}/bin-int/" .. OUTPUT_DIRECTORY .. "/%{prj.name}")

		filter "system:windows"
			systemversion "latest"
		
		filter "configurations:Debug"
			defines "Grapple_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "Grapple_RELEASE"
			runtime "Release"
			optimize "on"
			
		filter "configurations:Dist"
			defines "Grapple_DIST"
			runtime "Release"
			optimize "on"

	include(root_directory .. "GrappleCommon/premake5.lua")
	include(root_directory .. "GrappleScriptingCore/premake5.lua")
