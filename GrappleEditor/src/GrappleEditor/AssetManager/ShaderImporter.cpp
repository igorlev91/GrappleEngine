#include "ShaderImporter.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/ComputeShader.h"

#include "GrappleEditor/ShaderCompiler/ShaderCompiler.h"

namespace Grapple
{
	Ref<Asset> ShaderImporter::ImportShader(const AssetMetadata& metadata)
	{
		Grapple_PROFILE_FUNCTION();
		if (!ShaderCompiler::Compile(metadata.Handle))
			return false;

		Ref<Shader> shader = Shader::Create();
		shader->Handle = metadata.Handle;
		shader->Load();

		return shader;
	}

	Ref<Asset> ShaderImporter::ImportComputeShader(const AssetMetadata& metadata)
	{
		Grapple_PROFILE_FUNCTION();
		if (!ShaderCompiler::Compile(metadata.Handle))
			return false;

		Ref<ComputeShader> shader = ComputeShader::Create();
		shader->Handle = metadata.Handle;
		shader->Load();

		return shader;
	}
}
