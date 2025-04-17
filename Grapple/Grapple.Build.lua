local build_tool = require("BuildTool")

project "Grapple"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	build_tool.define_module("Grapple")
	build_tool.add_module_ref("GrapplePlatform")
	build_tool.add_module_ref("GrappleCore")
	build_tool.add_module_ref("GrappleECS")

    files
    {
        "src/**.h",
        "src/**.cpp",

		"vendor/stb_image/stb_image/**.h",
		"vendor/stb_image/stb_image/**.cpp",
    }

    includedirs
	{
		"src/",
		"%{wks.location}/GrappleCore/src/",
		"%{wks.location}/GrapplePlatform/src/",
		"%{wks.location}/GrappleECS/src/",

		INCLUDE_DIRS.GLAD,
		INCLUDE_DIRS.GLFW,
		INCLUDE_DIRS.glm,
		INCLUDE_DIRS.stb_image,
		INCLUDE_DIRS.spdlog,
		INCLUDE_DIRS.imguizmo,
		INCLUDE_DIRS.yaml_cpp,
		INCLUDE_DIRS.imgui,
	}

	links
	{
		"GLAD",
		"GLFW",
		"ImGUI",
		"GrappleECS",
		"GrappleCore",
		"GrapplePlatform",
		"yaml-cpp"
	}

	defines
	{
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"_GLFW_BUILD_DLL",
	}

	filter "system:windows"
		systemversion "latest"

		links { "dwmapi.lib" }

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
