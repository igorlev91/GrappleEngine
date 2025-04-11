include "Dependencies.lua"

workspace "Grapple"
	architecture "x86_64"
	startproject "GrappleEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

OUTPUT_DIRECTORY = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Grapple/vendor/GLFW"
    include "Grapple/vendor/GLAD"
    include "Grapple/vendor/ImGUI"
    include "Grapple/vendor/yaml-cpp"
group ""

group "Core"
	include "GrappleCommon/GrappleCommon.Build.lua"
	include "Grapple/Grapple.Build.lua"
	include "GrappleECS/GrappleECS.Build.lua"
	include "GrappleScriptingCore/GrappleScriptingCore.Build.lua"
group ""

group "Editor"
	include "GrappleEditor/GrappleEditor.Build.lua"
group ""
