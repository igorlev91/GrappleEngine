local M = {}

OUTPUT_DIRECTORY = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"

newoption
{
    trigger = "Grapple_root",
    description = "Grapple root directory"
}

local function set_module_defines(module_name, export)
	local module_api_define = string.format("%s_API", string.upper(module_name))

	local import_export = "Grapple_API_IMPORT";
	if export then
		import_export = "Grapple_API_EXPORT"
	end

	local dist_api = string.format("%s=", module_api_define)
	local modular_api = string.format("%s=%s", module_api_define, import_export)

	filter "configurations:not Dist"
		defines { modular_api }

	filter "Dist"
		defines { dist_api }

	filter {}

	targetdir("%{wks.location}/bin/" .. OUTPUT_DIRECTORY)
	objdir("%{wks.location}/bin-int/" .. OUTPUT_DIRECTORY .. "/%{prj.name}")
end

M.setup_workspace = function(name)
	workspace(name)
	configurations({"Debug", "Release", "Dist"})
	architecture("x86_64")
end

M.setup_project = function(name)
	local root = "%{wks.location}/" .. _OPTIONS["Grapple_root"]

	project(name)
	language("C++")
	cppdialect("C++17")
	staticruntime("off")
	files({"Source/**.h", "Source/*.hpp", "Source/*.cpp"})

	M.set_module_kind()

	includedirs({
		root .. "Grapple/vendor/spdlog/include",
		root .. "Grapple/vendor/glm",
	})

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

	filter {}

	M.define_module(name)
end

M.define_module = function(name)
	set_module_defines(name, true)
	M.set_module_kind()
end

M.add_module_ref = function(name)
	set_module_defines(name, false)
end

M.set_module_kind = function()
	filter "configurations:Debug or configurations:Release"
		kind "SharedLib"

	filter "configurations:Dist"
		kind "StaticLib"

	filter {}
end

M.add_internal_module_ref = function(name)
    set_module_defines(name);

    local root = "%{wks.location}/" .. _OPTIONS["Grapple_root"]

    includedirs
    {
        string.format("%s/%s/src/", root, name),
    }

    filter "configurations:not Dist"
        links
        {
            string.format("%s/bin/%s/%s.lib", root, OUTPUT_DIRECTORY, name),
        }

    filter {}
end

return M
