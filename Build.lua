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
    include "Grapple/vendor/ImGUI"
    include "Grapple/vendor/yaml-cpp"
	include "Grapple/vendor/msdf/MSDF.Build.lua"
group ""

group "Core"
	include "GrapplePlatform/GrapplePlatform.Build.lua"
	include "GrappleCore/GrappleCore.Build.lua"
	include "Grapple/Grapple.Build.lua"
	include "GrappleECS/GrappleECS.Build.lua"
group ""

group "Editor"
	include "GrappleEditor/GrappleEditor.Build.lua"
group ""
