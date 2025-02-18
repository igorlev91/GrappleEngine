project "Grapple"
    kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

	local GrappleIncludeDirs = {
        "src",
	}

	for i = 1, #INCLUDE_DIRS do
		GrappleIncludeDirs[#GrappleIncludeDirs+1] = INCLUDE_DIRS[i]
	end

    includedirs(GrappleIncludeDirs)

	links
	{
		"GLAD",
		"GLFW"
	}

	defines
	{
		"GLFW_INCLUDE_NONE"
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
