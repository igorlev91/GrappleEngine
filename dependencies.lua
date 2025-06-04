local vulkan_sdk = os.getenv("VULKAN_SDK")

if vulkan_sdk == nil then
	print("Couldn't find Vulkan SDK")
end

local vulkan_include = vulkan_sdk .. "/include/"
local vulkan_lib = vulkan_sdk .. "/lib/"

local assimp_path = "%{wks.location}/Grapple/vendor/assimp/assimp/"

INCLUDE_DIRS = {
	dds = "%{wks.location}/Grapple/vendor/dds-ktx/",
	GLFW = "%{wks.location}/Grapple/vendor/GLFW/include",
	glm = "%{wks.location}/Grapple/vendor/glm/",
	stb_image = "%{wks.location}/Grapple/vendor/stb_image",
	spdlog = "%{wks.location}/Grapple/vendor/spdlog/include",
	imgui = "%{wks.location}/Grapple/vendor/ImGUI/",
	imguizmo = "%{wks.location}/Grapple/vendor/ImGuizmo/",
	yaml_cpp = "%{wks.location}/Grapple/vendor/yaml-cpp/include/",
	vulkan_sdk = vulkan_include,
	vma = "%{wks.location}/Grapple/vendor/VMA/",

	msdf_gen = "%{wks.location}/Grapple/vendor/msdf/msdf-atlas-gen/msdfgen",
	msdf_atlas_gen = "%{wks.location}/Grapple/vendor/msdf/msdf-atlas-gen/msdf-atlas-gen",

	assimp = assimp_path .. "/include/",

	tracy = "%{wks.location}/Grapple/vendor/Tracy/tracy",
}

LIBRARIES = {
	vulkan = vulkan_lib .. "vulkan-1.lib",
	shaderc_debug = vulkan_lib .. "shaderc_sharedd.lib",
	spriv_cross_debug = vulkan_lib .. "spirv-cross-cored.lib",
	spriv_cross_glsl_debug = vulkan_lib .. "spirv-cross-glsld.lib",
	spriv_tools_debug = vulkan_lib .. "spirv-toolsd.lib",

	shaderc_release = vulkan_lib .. "shaderc_shared.lib",
	spriv_cross_release = vulkan_lib .. "spirv-cross-core.lib",
	spriv_cross_glsl_release = vulkan_lib .. "spirv-cross-glsl.lib",

	assimp_debug = assimp_path .. "/lib/Debug/assimp-vc143-mtd.lib",
	assimp_release = assimp_path .. "/lib/Release/assimp-vc143-mt.lib",

	assimp_zlib_debug = assimp_path .. "contrib/zlib/Debug/zlibstaticd.lib",
	assimp_zlib_release = assimp_path .. "contrib/zlib/Release/zlibstatic.lib",
}
