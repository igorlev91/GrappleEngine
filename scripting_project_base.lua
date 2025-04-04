PROJECT_OUTPUT_DIRECTORY = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"

local function get_Grapple_root()
	return _OPTIONS["Grapple_root"]
end

local function get_project_binary_location(project_name)
	return get_Grapple_root() .. "/bin/" .. PROJECT_OUTPUT_DIRECTORY .. project_name
end

local function get_library_location (project_name)
	return get_project_binary_location(project_name) .. "/" .. project_name .. ".lib"
end

return {
	setup = function ()
		newoption
		{
			trigger = "Grapple_root",
			description = "Grapple root directory"
		}

		include(get_Grapple_root() .. "dependencies.lua")
	end,
	get_project_binary_location = get_project_binary_location,
	get_library_location = get_library_location,
	create_workspace = function(workspace_name)
		workspace(workspace_name)
			defines
			{
				"Grapple_BUILD_SHARED"
			}

			architecture "x86_64"

			configurations
			{
				"Debug",
				"Release",
				"Dist",
			}

			flags
			{
				"MultiProcessorCompile"
			}
	end,
	create_project = function(project_name, config_func)
		local dependecies_location = get_Grapple_root();

		project(project_name)
			kind "SharedLib"
			language "C++"
			cppdialect "C++17"
			staticruntime "off"

			files
			{
				"Assets/**.h",
				"Assets/**.cpp",
			}

			includedirs
			{
				"src/",

				dependecies_location .. "GrappleScriptingCore/src/",
				dependecies_location .. "GrappleECS/include/",
				dependecies_location .. "GrappleCommon/src/",
				dependecies_location .. "Grapple/vendor/spdlog/include/",

                dependecies_location .. "Grapple/vendor/glm/"
			}

			links
			{
				get_library_location("GrappleCommon"),
				get_library_location("GrappleScriptingCore"),
			}

			targetdir("%{wks.location}/bin/" .. PROJECT_OUTPUT_DIRECTORY .. "/%{prj.name}")
			objdir("%{wks.location}/bin-int/" .. PROJECT_OUTPUT_DIRECTORY .. "/%{prj.name}")

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

		if config_func ~= nil then
			config_func()
		end
	end
}
