local build_tool = include(_OPTIONS["Grapple_root"] .. "BuildTool.lua")

build_tool.setup_project("Sandbox")

build_tool.add_internal_module_ref("Grapple")
build_tool.add_internal_module_ref("GrappleCommon")
build_tool.add_internal_module_ref("GrappleECS")
