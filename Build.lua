include "Dependencies.lua"
include "BuildTool.lua"

workspace "Grapple"
	architecture "x86_64"
	startproject "GrappleEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist",
	}

	filter "configurations:not Dist"
		disablewarnings
		{
			"4251"
		}

	flags
	{
		"MultiProcessorCompile"
	}

group "Dependencies"
    include "Grapple/vendor/GLFW"
    include "Grapple/vendor/GLAD"
    include "Grapple/vendor/ImGUI"
    include "Grapple/vendor/yaml-cpp"
group ""

group "Core"
	include "GrapplePlatform/GrapplePlatform.Build.lua"
	include "GrappleCommon/GrappleCommon.Build.lua"
	include "Grapple/Grapple.Build.lua"
	include "GrappleECS/GrappleECS.Build.lua"
	include "GrappleScriptingCore/GrappleScriptingCore.Build.lua"
group ""

group "Editor"
	include "GrappleEditor/GrappleEditor.Build.lua"
group ""
