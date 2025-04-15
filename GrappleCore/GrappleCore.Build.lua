local build_tool = require("BuildTool")

project "GrappleCore"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	build_tool.define_module("GrappleCore")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
	{
		"src/",
		INCLUDE_DIRS.spdlog,
		INCLUDE_DIRS.glm,
	}

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
