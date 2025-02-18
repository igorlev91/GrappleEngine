project "GRAPPLE"
    kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "src"
    }

	targetdir("%{wks.location}/bin/" .. OUTPUT_DIRECTORY .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. OUTPUT_DIRECTORY .. "/%{prj.name}")

	filter "system:windows"
		systemversion "latest"
	
	filter "configurations:Debug"
		defines "GRAPPLE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GRAPPLE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GRAPPLE_DIST"
		runtime "Release"
		optimize "on"
