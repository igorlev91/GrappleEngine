project "GrappleEditor"
    kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
		"%{wks.location}/GrappleCommon/src",
		"%{wks.location}/GrapplePlatform/src",
		"%{wks.location}/GrappleECS/src",
		"%{wks.location}/GrappleECS/include",
		"%{wks.location}/GrappleScriptingCore/src/",

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
		"ImGUI",
		"GrappleCommon",
		"GrapplePlatform",
		"GrappleScriptingCore",
		"GrappleECS",
		"yaml-cpp",
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"Grapple_SCRIPTING_CORE_NO_MACROS",
	}

	debugargs
	{
		"%{wks.location}/Sandbox/Sandbox.Grappleproj"
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

