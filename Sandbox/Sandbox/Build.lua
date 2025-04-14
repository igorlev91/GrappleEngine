OUTPUT_DIRECTORY = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"

local build_tool = include(_OPTIONS["Grapple_root"] .. "BuildTool.lua")

workspace "Sandbox"
	language "C++"
	cppdialect "c++17"
	architecture "x86_64"
	configurations { "Debug", "Release", "Dist" }

	project "Sandbox"
		files { "Source/**.h", "Source/**.cpp" }

	includedirs
	{
		_OPTIONS["Grapple_root"] .. "Grapple/vendor/spdlog/include",
		_OPTIONS["Grapple_root"] .. "Grapple/vendor/glm/",
	}

build_tool.set_module_kind()

build_tool.define_module("Sandbox")
build_tool.add_internal_module_ref("Grapple")
build_tool.add_internal_module_ref("GrappleCommon")
build_tool.add_internal_module_ref("GrappleECS")
