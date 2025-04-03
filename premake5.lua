include "dependencies.lua"

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
	include "GrappleCommon"
	include "Grapple"
	include "GrappleECS"
	include "GrappleScriptingCore"
group ""

group "Editor"
	include "GrappleEditor"
group ""
