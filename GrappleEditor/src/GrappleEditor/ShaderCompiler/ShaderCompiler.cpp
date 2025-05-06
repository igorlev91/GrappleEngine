#include "ShaderCompiler.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "GrappleEditor/AssetManager/EditorShaderCache.h"
#include "GrappleEditor/ShaderCompiler/ShaderSourceParser.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <fstream>
#include <string>
#include <sstream>

namespace Grapple
{
    struct PreprocessedShaderProgram
    {
        std::string Source;
        ShaderStageType Stage;
    };

    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        virtual shaderc_include_result* GetInclude(const char* requested_source,
            shaderc_include_type type,
            const char* requesting_source,
            size_t include_depth)
        {
            std::filesystem::path requestingPath = requesting_source;
            std::filesystem::path parent = requestingPath.parent_path();
            std::filesystem::path includedFilePath = parent / requested_source;

            if (!std::filesystem::exists(includedFilePath))
                includedFilePath = std::filesystem::path("assets/Shaders/") / requested_source;

            shaderc_include_result* includeData = new shaderc_include_result();
            includeData->content = nullptr;
            includeData->content_length = 0;
            includeData->source_name = nullptr;
            includeData->source_name_length = 0;
            includeData->user_data = nullptr;

            std::ifstream inputStream(includedFilePath);

            std::string_view path;
            if (inputStream.is_open())
            {
                std::string pathString = includedFilePath.string();
                path = pathString;

                char* sourceName = new char[path.size() + 1];
                sourceName[path.size()] = 0;

                memcpy_s(sourceName, path.size() + 1, pathString.c_str(), pathString.size());

                includeData->source_name = sourceName;
                includeData->source_name_length = pathString.size();
            }

            if (inputStream.is_open())
            {
                inputStream.seekg(0, std::ios::end);
                size_t size = inputStream.tellg();
                char* source = new char[size + 1];
                std::memset(source, 0, size + 1);

                inputStream.seekg(0, std::ios::beg);
                inputStream.read(source, size);

                includeData->content = source;
                includeData->content_length = strlen(source);
            }
            else
            {
                std::string_view message = "File not found.";

                includeData->content = message.data();
                includeData->content_length = message.size();
            }

            return includeData;
        }

        virtual void ReleaseInclude(shaderc_include_result* data)
        {
            if (data->source_name)
            {
                delete data->source_name;
                delete data->content;
            }

            delete data;
        }
    };

    static std::optional<std::vector<uint32_t>> CompileVulkanGlslToSpirv(const std::string& path, const std::string& source, shaderc_shader_kind programKind)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetGenerateDebugInfo();
        options.SetIncluder(CreateScope<ShaderIncluder>());
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::SpvCompilationResult shaderModule = compiler.CompileGlslToSpv(source.c_str(), source.size(), programKind, path.c_str(), "main", options);
        if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            std::string_view stageName = "";
            switch (programKind)
            {
            case shaderc_vertex_shader:
                stageName = "Vertex";
                break;
            case shaderc_fragment_shader:
                stageName = "Pixel";
                break;
            }

            Grapple_CORE_ERROR("Failed to compile Vulkan GLSL '{}' Stage = {}", path, stageName);
            Grapple_CORE_ERROR("Shader Error: {0}", shaderModule.GetErrorMessage());
            return {};
        }

        return std::vector<uint32_t>(shaderModule.cbegin(), shaderModule.cend());
    }

    static std::optional<std::vector<uint32_t>> CompileSpirvToGlsl(const std::string& path, const std::vector<uint32_t>& spirvData, shaderc_shader_kind programKind)
    {
        spirv_cross::CompilerGLSL glslCompiler(spirvData);
        std::string glsl = glslCompiler.compile();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetAutoMapLocations(true);
        options.SetGenerateDebugInfo();
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::SpvCompilationResult shaderModule = compiler.CompileGlslToSpv(glsl, programKind, path.c_str(), options);
        if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            std::string_view stageName = "";
            switch (programKind)
            {
            case shaderc_vertex_shader:
                stageName = "Vertex";
                break;
            case shaderc_fragment_shader:
                stageName = "Pixel";
                break;
            }

            Grapple_CORE_ERROR("Failed to compile shader '{}' Stage = {}", path, stageName);
            Grapple_CORE_ERROR("Shader Error: {0}", shaderModule.GetErrorMessage());
            return {};
        }

        return std::vector<uint32_t>(shaderModule.cbegin(), shaderModule.cend());
    }

    static void PrintError(const ShaderError& error)
    {
        if (error.Position.Line == UINT32_MAX || error.Position.Column == UINT32_MAX)
            Grapple_CORE_ERROR(error.Message);
        else
        {
            Grapple_CORE_ERROR("Line {} Column {}: {}",
                error.Position.Line,
                error.Position.Column,
                error.Message);
        }
    }

    static std::optional<bool> BoolFromString(std::string_view string)
    {
        if (string == "true")
            return true;
        if (string == "false")
            return false;
        return {};
    }

    static void Preprocess(const std::filesystem::path& shaderPath,
        std::string_view source,
        std::vector<PreprocessedShaderProgram>& programs,
        ShaderFeatures& features,
        SerializableObjectDescriptor& serializationDescriptor,
        std::vector<ShaderError>& errors,
        std::unordered_map<std::string_view, size_t>& propertyNameToIndex)
    {
        ShaderSourceParser parser(shaderPath, source, errors);
        parser.Parse();

        if (errors.size() > 0)
            return;

        for (const auto& sourceBlock : parser.GetSourceBlocks())
        {
            PreprocessedShaderProgram& program = programs.emplace_back();
            program.Source = sourceBlock.Source;
            program.Stage = sourceBlock.Stage;
        }

        const Block& rootBlock = parser.GetBlock(0);
        for (const auto& element : rootBlock.Elements)
        {
            if (element.Name.Value == "Culling")
            {
                auto culling = CullingModeFromString(element.Value.Value);
                if (culling)
                    features.Culling = *culling;
                else
                    errors.emplace_back(element.Value.Position, fmt::format("Unknown culling mode '{}'", element.Value.Value));
            }
            else if (element.Name.Value == "BlendMode")
            {
                auto blending = BlendModeFromString(element.Value.Value);
                if (blending)
                    features.Blending = *blending;
                else
                    errors.emplace_back(element.Value.Position, fmt::format("Unknown blending mode '{}'", element.Value.Value));
            }
            else if (element.Name.Value == "DepthTest")
            {
                std::optional<bool> value = BoolFromString(element.Value.Value);
                if (value)
                    features.DepthTesting = *value;
                else
                    errors.emplace_back(element.Value.Position, fmt::format("Expected bool, but got {}", element.Value.Value));
            }
            else if (element.Name.Value == "DepthWrite")
            {
                std::optional<bool> value = BoolFromString(element.Value.Value);
                if (value)
                    features.DepthWrite = *value;
                else
                    errors.emplace_back(element.Value.Position, fmt::format("Expected bool, but got {}", element.Value.Value));
            }
            else if (element.Name.Value == "DepthFunction")
            {
                auto depthFunction = DepthComparisonFunctionFromString(element.Value.Value);
                if (depthFunction)
                    features.DepthFunction = *depthFunction;
                else
                    errors.emplace_back(element.Value.Position, fmt::format("Unknown depth function '{}'", element.Value.Value));
            }
            else if (element.Name.Value == "Properties")
            {
                if (element.Type != BlockElementType::Block)
                    errors.emplace_back(element.Value.Position, fmt::format("Expected an array of properties, instead of a value"));
                else
                {
                    const Block& block = parser.GetBlock(element.ChildBlockIndex);
                    for (const BlockElement& property : block.Elements)
                    {
                        SerializablePropertyDescriptor& propertyDescriptor = serializationDescriptor.Properties.emplace_back(
                            property.Name.Value, SIZE_MAX,
                            SerializablePropertyType::Int);

                        propertyNameToIndex.emplace(propertyDescriptor.Name, serializationDescriptor.Properties.size() - 1);
                    }
                }
            }
        }
    }

    static void Reflect(spirv_cross::Compiler& compiler,
        SerializableObjectDescriptor& serializationDescriptor,
        const std::unordered_map<std::string_view, size_t>& propertyNameToIndex)
    {
        const spirv_cross::ShaderResources& resources = compiler.get_shader_resources();
      
        size_t lastPropertyOffset = 0;
        for (const auto& resource : resources.push_constant_buffers)
        {
            const auto& bufferType = compiler.get_type(resource.base_type_id);
            size_t bufferSize = compiler.get_declared_struct_size(bufferType);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

            uint32_t membersCount = (uint32_t)bufferType.member_types.size();

            for (uint32_t i = 0; i < membersCount; i++)
            {
                const std::string& memberName = compiler.get_member_name(resource.base_type_id, i);
                spirv_cross::TypeID memberTypeId = bufferType.member_types[i];
                size_t offset = compiler.type_struct_member_offset(bufferType, i);

                const auto& memberType = compiler.get_type(memberTypeId);

                SerializablePropertyType dataType = SerializablePropertyType::Int;
                uint32_t componentsCount = memberType.vecsize;

                bool error = false;

                switch (memberType.basetype)
                {
                case spirv_cross::SPIRType::BaseType::Int:
                    switch (componentsCount)
                    {
                    case 1:
                        dataType = SerializablePropertyType::Int;
                        break;
                    case 2:
                        dataType = SerializablePropertyType::IntVector2;
                        break;
                    case 3:
                        dataType = SerializablePropertyType::IntVector3;
                        break;
                    case 4:
                        dataType = SerializablePropertyType::IntVector4;
                        break;
                    default:
                        error = true;
                        Grapple_CORE_ERROR("Unsupported components count");
                    }

                    break;
                case spirv_cross::SPIRType::BaseType::Float:
                    switch (componentsCount)
                    {
                    case 1:
                        dataType = SerializablePropertyType::Float;
                        break;
                    case 2:
                        dataType = SerializablePropertyType::FloatVector2;
                        break;
                    case 3:
                        dataType = SerializablePropertyType::FloatVector3;
                        break;
                    case 4:
                        if (memberType.columns == 4)
                            dataType = SerializablePropertyType::Matrix4x4;
                        else
                            dataType = SerializablePropertyType::FloatVector4;
                        break;
                    default:
                        error = true;
                        Grapple_CORE_ERROR("Unsupported components count");
                    }

                    break;
                default:
                    error = true;
                    Grapple_CORE_ERROR("Unsupported shader data type");
                }

                if (error)
                    continue;

                SerializablePropertyDescriptor* property = nullptr;
                if (resource.name.empty())
                {
                    auto propertyIterator = propertyNameToIndex.find(resource.name);
                    if (propertyIterator != propertyNameToIndex.end())
                        property = &serializationDescriptor.Properties[propertyIterator->second];
                }
                else
                {
                    auto propertyIterator = propertyNameToIndex.find(fmt::format("{0}.{1}", resource.name, memberName));
                    if (propertyIterator != propertyNameToIndex.end())
                        property = &serializationDescriptor.Properties[propertyIterator->second];
                }

                if (property)
                {
                    property->PropertyType = dataType;
                    property->Offset = offset;
                }

                lastPropertyOffset = offset + compiler.get_declared_struct_member_size(bufferType, i);
            }
        }

        for (const auto& resource : resources.sampled_images)
        {
            const auto& samplerType = compiler.get_type(resource.type_id);
            uint32_t membersCount = (uint32_t)samplerType.member_types.size();

            ShaderDataType type = ShaderDataType::Sampler;
            size_t size = ShaderDataTypeSize(type);
            if (samplerType.array.size() > 0)
            {
                if (samplerType.array.size() == 1)
                {
                    type = ShaderDataType::SamplerArray;
                    size *= samplerType.array[0];
                }
                else
                {
                    Grapple_CORE_ERROR("Unsupported sampler array dimensions: {0}", samplerType.array.size());
                    continue;
                }
            }

            auto propertyIterator = propertyNameToIndex.find(resource.name);
            if (propertyIterator != propertyNameToIndex.end())
            {
                SerializablePropertyDescriptor& propertyDescriptor = serializationDescriptor.Properties[propertyIterator->second];
                propertyDescriptor.PropertyType = SerializablePropertyType::Texture2D;
                propertyDescriptor.Offset = lastPropertyOffset;
            }

            lastPropertyOffset += sizeof(AssetHandle);
        }
    }
    
    bool ShaderCompiler::Compile(AssetHandle shaderHandle, bool forceRecompile)
    {
        Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(shaderHandle));

        const std::filesystem::path& shaderPath = AssetManager::GetAssetMetadata(shaderHandle)->Path;
        std::string sourceString = "";
        {
            std::ifstream file(shaderPath);
            if (!file.is_open())
            {
                Grapple_CORE_ERROR("Failed not read shader file {0}", shaderPath.string());
                return false;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            sourceString = buffer.str();
        }

        std::string pathString = shaderPath.string();
        std::vector<PreprocessedShaderProgram> programs;
        SerializableObjectDescriptor serializationDescriptor;
        std::unordered_map<std::string_view, size_t> propertyNameToIndex;

        std::vector<ShaderError> errors;

        ShaderFeatures shaderFeatures;
        std::string_view source = sourceString;

        Preprocess(shaderPath, source, programs, shaderFeatures, serializationDescriptor, errors, propertyNameToIndex);
        if (errors.size() > 0)
        {
            Grapple_CORE_ERROR("Failed to compile shader '{}'", pathString);
            for (const ShaderError& error : errors)
                PrintError(error);

            return false;
        }

        for (const PreprocessedShaderProgram& program : programs)
        {
            shaderc_shader_kind shaderKind = (shaderc_shader_kind)0;
            switch (program.Stage)
            {
            case ShaderStageType::Vertex:
                shaderKind = shaderc_vertex_shader;
                break;
            case ShaderStageType::Pixel:
                shaderKind = shaderc_fragment_shader;
                break;
            }

            std::optional<std::vector<uint32_t>> compiledVulkanShader = {};

            if (!forceRecompile)
            {
                compiledVulkanShader = ShaderCacheManager::GetInstance()->FindCache(
                    shaderHandle,
                    ShaderTargetEnvironment::Vulkan,
                    program.Stage);
            }

            if (!compiledVulkanShader)
            {
                compiledVulkanShader = CompileVulkanGlslToSpirv(pathString, program.Source, shaderKind);
                if (!compiledVulkanShader)
                    return false;

                ShaderCacheManager::GetInstance()->SetCache(shaderHandle, ShaderTargetEnvironment::Vulkan, program.Stage, compiledVulkanShader.value());
            }

            if (serializationDescriptor.Properties.size() > 0)
            {
                try
                {
                    std::unordered_map<std::string_view, size_t> propertyNameToIndex;
                    for (size_t i = 0; i < serializationDescriptor.Properties.size(); i++)
                        propertyNameToIndex.emplace(serializationDescriptor.Properties[i].Name, i);

                    spirv_cross::Compiler compiler(compiledVulkanShader.value());
                    Reflect(compiler, serializationDescriptor, propertyNameToIndex);
                }
                catch (spirv_cross::CompilerError& e)
                {
                    Grapple_CORE_ERROR("Shader reflection failed: {}", e.what());
                }
            }

            std::optional<std::vector<uint32_t>> compiledOpenGLShader = {};
            
            if (!forceRecompile)
            {
                compiledOpenGLShader = ShaderCacheManager::GetInstance()->FindCache(
                    shaderHandle,
                    ShaderTargetEnvironment::OpenGL,
                    program.Stage);
            }

            if (!compiledOpenGLShader)
            {
                compiledOpenGLShader = CompileSpirvToGlsl(pathString, compiledVulkanShader.value(), shaderKind);
                if (!compiledOpenGLShader)
                    return false;

                ShaderCacheManager::GetInstance()->SetCache(shaderHandle, ShaderTargetEnvironment::OpenGL, program.Stage, compiledOpenGLShader.value());
            }
        }

        size_t insertionIndex = 0;
        for (size_t i = 0; i < serializationDescriptor.Properties.size(); i++)
        {
            if (serializationDescriptor.Properties[i].Offset == SIZE_MAX)
                continue;

            if (insertionIndex != i)
                serializationDescriptor.Properties[insertionIndex] = std::move(serializationDescriptor.Properties[i]);

            insertionIndex++;
        }

        serializationDescriptor.Properties.erase(
            serializationDescriptor.Properties.begin() + insertionIndex,
            serializationDescriptor.Properties.end());

        ((EditorShaderCache*)ShaderCacheManager::GetInstance().get())->SetShaderEntry(
            shaderHandle,
            shaderFeatures,
            std::move(serializationDescriptor));

        return true;
    }
}
