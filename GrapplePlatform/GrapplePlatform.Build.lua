local build_tool = require("BuildTool")

project "GrapplePlatform"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	build_tool.define_module("GrapplePlatform")
	build_tool.add_module_ref("GrappleCore")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
	{
		"src/",
		"%{wks.location}/GrappleCore/src/",

		INCLUDE_DIRS.GLFW,
		INCLUDE_DIRS.glm,
		INCLUDE_DIRS.spdlog,
	}

	links
	{
		"GLFW",
		"ImGUI",
		"GrappleCore",
	}

	defines
	{
		"GLFW_INCLUDE_NONE",
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
