local build_tool = require("BuildTool")

project "GrappleEditor"
    kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	build_tool.add_module_ref("Grapple")
	build_tool.add_module_ref("GrapplePlatform")
	build_tool.add_module_ref("GrappleCore")
	build_tool.add_module_ref("GrappleECS")

    files
    {
        "src/**.h",
        "src/**.cpp",

		"%{wks.location}/Grapple/vendor/ImGuizmo/ImGuizmo.h",
		"%{wks.location}/Grapple/vendor/ImGuizmo/ImGuizmo.cpp",
    }

    includedirs
    {
        "src",
		"%{wks.location}/Grapple/src",
		"%{wks.location}/GrappleCore/src",
		"%{wks.location}/GrapplePlatform/src",
		"%{wks.location}/GrappleECS/src",

		INCLUDE_DIRS.msdf_gen,
		INCLUDE_DIRS.msdf_atlas_gen,

		INCLUDE_DIRS.assimp,

		INCLUDE_DIRS.glm,
		INCLUDE_DIRS.GLFW,
		INCLUDE_DIRS.spdlog,
		INCLUDE_DIRS.imgui,
		INCLUDE_DIRS.imguizmo,
		INCLUDE_DIRS.yaml_cpp,
    }

	links
	{
		"Grapple",
		"GLFW",
		"ImGUI",
		"GrappleCore",
		"GrapplePlatform",
		"GrappleECS",
		"yaml-cpp",
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
	}

	debugargs
	{
		"%{wks.location}/Sandbox/Sandbox.Grappleproj"
	}

	targetdir("%{wks.location}/bin/" .. OUTPUT_DIRECTORY)
	objdir("%{wks.location}/bin-int/" .. OUTPUT_DIRECTORY .. "/%{prj.name}")

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "Grapple_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			LIBRARIES.assimp_debug,
			LIBRARIES.assimp_zlib_debug,
		}

	filter "configurations:Release"
		defines "Grapple_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			LIBRARIES.assimp_release,
			LIBRARIES.assimp_zlib_release,
		}

	filter "configurations:Dist"
		defines "Grapple_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			LIBRARIES.assimp_release,
			LIBRARIES.assimp_zlib_release,
		}
