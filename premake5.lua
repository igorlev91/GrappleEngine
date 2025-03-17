include "dependencies.lua"

workspace "Grapple"
	architecture "x86_64"
	startproject "Sandbox"

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
group ""

group "Core"
	include "Grapple"
group ""

group "Sandbox"
	include "Sandbox"
group ""
