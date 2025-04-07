#pragma once

#include "Grapple/Core/UUID.h"
#include "Grapple/Scripting/ScriptingModule.h"

#include "GrappleECS/World.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"
#include "GrappleScriptingCore/Bindings/ECS/SystemInfo.h"

#include <filesystem>

namespace Grapple
{
	using ModuleEventFunction = void(*)(Internal::ModuleConfiguration&);

	struct ScriptingTypeInstance
	{
		size_t TypeIndex = SIZE_MAX;
		void* Instance = nullptr;
	};

	struct ScriptingModuleData
	{
		ScriptingModule Module;
		Internal::ModuleConfiguration Config;

		std::optional<ModuleEventFunction> OnLoad;
		std::optional<ModuleEventFunction> OnUnload;

		std::vector<ScriptingTypeInstance> ScriptingInstances;
		std::unordered_map<std::string_view, size_t> TypeNameToIndex;
	};

	class ScriptingEngine
	{
	public:
		struct Data
		{
			World* CurrentWorld = nullptr;
			bool ShouldRegisterComponents = false;
			std::vector<ScriptingModuleData> Modules;
			std::vector<ComponentId> TemporaryQueryComponents;

			struct TypeIndex
			{
				size_t ModuleIndex;
				size_t TypeIndex;
			};

			std::unordered_map<ComponentId, TypeIndex> ComponentIdToTypeIndex;
		};
	public:
		static void Initialize();
		static void Shutdown();

		static void OnFrameStart(float deltaTime);

		static void SetCurrentECSWorld(World& world);

		static void LoadModules();
		static void ReleaseScriptingInstances();

		static void UnloadAllModules();

		static void RegisterComponents();
		static void RegisterSystems();

		static std::optional<const Internal::ScriptingType*> FindComponentType(ComponentId id);

		inline static Data& GetData() { return s_Data; }
	private:
		static void LoadModule(const std::filesystem::path& modulePath);
	private:
		static Data s_Data;

		static const std::string s_ModuleLoaderFunctionName;
		static const std::string s_ModuleUnloaderFunctionName;
	};
}