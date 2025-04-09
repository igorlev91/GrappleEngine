#include "ScriptingEngine.h"

#include "Grapple/Core/Log.h"

#include "Grapple/Scripting/ScriptingModule.h"
#include "Grapple/Scripting/ScriptingBridge.h"
#include "Grapple/Scene/Components.h"
#include "Grapple/Project/Project.h"

#include "GrappleScriptingCore/Bindings/ECS/SystemInfo.h"
#include "GrappleScriptingCore/Bindings/ECS/ComponentInfo.h"

#include "GrappleECS.h"

namespace Grapple
{
	const std::string ScriptingEngine::s_ModuleLoaderFunctionName = "OnModuleLoaded";
	const std::string ScriptingEngine::s_ModuleUnloaderFunctionName = "OnModuleUnloaded";

	ScriptingEngine::Data ScriptingEngine::s_Data;

	void ScriptingEngine::Initialize()
	{
	}

	void ScriptingEngine::Shutdown()
	{
		UnloadAllModules();
	}

	void ScriptingEngine::OnFrameStart(float deltaTime)
	{
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			module.Config.TimeData->DeltaTime = deltaTime;
		}
	}

	void ScriptingEngine::SetCurrentECSWorld(World& world)
	{
		s_Data.CurrentWorld = &world;
		ScriptingBridge::SetCurrentWorld(world);
	}

	void ScriptingEngine::LoadModules()
	{
		Grapple_CORE_ASSERT(Project::GetActive());

		std::string_view configurationName = "";
		std::string_view platformName = "";
#ifdef Grapple_DEBUG
		configurationName = "Debug";
#elif defined(Grapple_RELEASE)
		configurationName = "Release";
#elif defined(Grapple_DIST)
		configurationName = "Dist";
#endif

#ifdef Grapple_PLATFORM_WINDOWS
		platformName = "windows";
#endif

		for (const std::string& moduleName : Project::GetActive()->ScriptingModules)
		{
			ScriptingEngine::LoadModule(Project::GetActive()->Location
				/ fmt::format("bin/{0}-{1}-x86_64/{2}/{2}.dll", configurationName, platformName, moduleName));
		}

		s_Data.ShouldRegisterComponents = true;
	}

	void ScriptingEngine::ReleaseScriptingInstances()
	{
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			for (ScriptingTypeInstance& instance : module.ScriptingInstances)
			{
				if (instance.Instance != nullptr)
				{
					const Internal::ScriptingType* type = GetScriptingType(instance.Type);
					type->Deleter(instance.Instance);
				}
			}

			module.ScriptingInstances.clear();
		}
	}

	void ScriptingEngine::LoadModule(const std::filesystem::path& modulePath)
	{
		ScriptingModuleData moduleData;
		moduleData.Module.Load(modulePath);

		if (!moduleData.Module.IsLoaded())
			return;

		if (moduleData.Module.IsLoaded())
		{
			size_t moduleIndex = s_Data.Modules.size();

			moduleData.Config = Internal::ModuleConfiguration{};

			moduleData.OnLoad = moduleData.Module.LoadFunction<ModuleEventFunction>(s_ModuleLoaderFunctionName);
			moduleData.OnUnload = moduleData.Module.LoadFunction<ModuleEventFunction>(s_ModuleUnloaderFunctionName);

			if (moduleData.OnLoad.has_value())
			{
				moduleData.OnLoad.value()(moduleData.Config);

				const std::vector<Internal::ScriptingType*>& registeredTypes = *moduleData.Config.RegisteredTypes;

				size_t typeIndex = 0;
				for (Internal::ScriptingType* type : registeredTypes)
				{
					s_Data.TypeNameToIndex.emplace(type->Name, ScriptingItemIndex(moduleIndex, typeIndex));

					if (type->ConfigureSerialization)
						type->ConfigureSerialization(type->GetSerializationSettings());

					typeIndex++;
				}

				ScriptingBridge::ConfigureModule(moduleData.Config);
			}

			s_Data.Modules.push_back(std::move(moduleData));
		}
	}

	const Internal::ScriptingType* ScriptingEngine::GetScriptingType(ScriptingItemIndex index)
	{
		return (*s_Data.Modules[index.ModuleIndex].Config.RegisteredTypes)[index.IndexInModule];
	}

	void ScriptingEngine::UnloadAllModules()
	{
		s_Data.ShouldRegisterComponents = false;
		ReleaseScriptingInstances();

		for (auto& module : s_Data.Modules)
		{
			if (module.OnUnload.has_value())
				module.OnUnload.value()(module.Config);
		}

		s_Data.ComponentIdToTypeIndex.clear();
		s_Data.TemporaryQueryComponents.clear();
		s_Data.Modules.clear();
		s_Data.TypeNameToIndex.clear();
		s_Data.SystemIndexToInstance.clear();
		s_Data.SystemNameToInstance.clear();
		s_Data.CurrentWorld = nullptr;
	}

	void ScriptingEngine::RegisterComponents()
	{
		if (s_Data.Modules.size() == 0)
			return;
		if (!s_Data.ShouldRegisterComponents)
			return;

		Grapple_CORE_ASSERT(s_Data.CurrentWorld != nullptr);

		size_t moduleIndex = 0;
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			Grapple_CORE_ASSERT(module.Module.IsLoaded());
			Grapple_CORE_ASSERT(module.Config.RegisteredComponents != nullptr);
			Grapple_CORE_ASSERT(module.Config.RegisteredTypes != nullptr);
			for (Internal::ComponentInfo* component : *module.Config.RegisteredComponents)
			{
				auto typeIndexIterator = s_Data.TypeNameToIndex.find(std::string(component->Name));
				Grapple_CORE_ASSERT(typeIndexIterator != s_Data.TypeNameToIndex.end());
				Grapple_CORE_ASSERT(typeIndexIterator->second.ModuleIndex == moduleIndex);

				const Internal::ScriptingType* type = (*module.Config.RegisteredTypes)[typeIndexIterator->second.IndexInModule];

				if (component->AliasedName.has_value())
				{
					std::optional<ComponentId> aliasedComponent = s_Data.CurrentWorld->GetRegistry().FindComponnet(component->AliasedName.value());
					Grapple_CORE_ASSERT(aliasedComponent.has_value(), "Failed to find component");
					component->Id = aliasedComponent.value();
				}
				else
				{
					component->Id = s_Data.CurrentWorld->GetRegistry().RegisterComponent(component->Name, type->Size, type->Destructor);
					s_Data.ComponentIdToTypeIndex.emplace(component->Id, typeIndexIterator->second);
				}
			}

			moduleIndex++;
		}

		s_Data.ShouldRegisterComponents = false;
	}

	void ScriptingEngine::CreateSystems()
	{
		size_t moduleIndex = 0;
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			module.FirstSystemInstance = module.ScriptingInstances.size();
			for (const Internal::SystemInfo* systemInfo : *module.Config.RegisteredSystems)
			{
				auto typeIndexIterator = s_Data.TypeNameToIndex.find(std::string(systemInfo->Name));
				Grapple_CORE_ASSERT(typeIndexIterator != s_Data.TypeNameToIndex.end());

				const Internal::ScriptingType* type = (*module.Config.RegisteredTypes)[typeIndexIterator->second.IndexInModule];
				Internal::SystemBase* systemInstance = (Internal::SystemBase*)type->Constructor();

				s_Data.SystemNameToInstance.emplace(systemInfo->Name, ScriptingItemIndex(moduleIndex, module.ScriptingInstances.size()));
				module.ScriptingInstances.push_back(ScriptingTypeInstance{ typeIndexIterator->second, systemInstance });
			}

			moduleIndex++;
		}
	}

	void ScriptingEngine::RegisterSystems()
	{
		std::string_view defaultGroupName = "Scripting Update";
		std::optional<SystemGroupId> defaultGroup = s_Data.CurrentWorld->GetSystemsManager().FindGroup(defaultGroupName);
		Grapple_CORE_ASSERT(defaultGroup.has_value());

		SystemsManager& systems = s_Data.CurrentWorld->GetSystemsManager();
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			size_t instanceIndex = module.FirstSystemInstance;
			for (Internal::SystemInfo* systemInfo : *module.Config.RegisteredSystems)
			{
				ScriptingTypeInstance& instance = module.ScriptingInstances[instanceIndex];
				Internal::SystemBase& systemInstance = instance.As<Internal::SystemBase>();

				const Internal::ScriptingType* type = GetScriptingType(instance.Type);

				SystemId id = systems.RegisterSystem(type->Name,
					nullptr,
					[system = &systemInstance](SystemExecutionContext& context)
					{
						for (EntityView view : context.GetQuery())
						{
							Grapple_CORE_ASSERT(s_Data.CurrentWorld != nullptr);
							ArchetypeRecord& record = s_Data.CurrentWorld->GetRegistry().GetArchetypeRecord(view.GetArchetype());

							size_t chunksCount = record.Storage.GetChunksCount();
							for (size_t i = 0; i < chunksCount; i++)
							{
								Internal::EntityView chunk(
									view.GetArchetype(),
									record.Storage.GetChunkBuffer(i),
									record.Storage.GetEntitySize(),
									record.Storage.GetEntitiesCountInChunk(i));

								system->Execute(chunk);
							}
						}
					},
					nullptr);

				systemInfo->Id = id;

				s_Data.SystemIndexToInstance.emplace(id, ScriptingItemIndex(
					instance.Type.ModuleIndex,
					instanceIndex));

				s_Data.TemporaryQueryComponents.clear();
				instanceIndex++;
			}
		}

		std::vector<Internal::SystemConfiguration> configs;
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			const auto& registeredSystems = *module.Config.RegisteredSystems;
			for (size_t systemIndex = 0; systemIndex < registeredSystems.size(); systemIndex++)
			{
				const Internal::SystemInfo* systemInfo = registeredSystems[systemIndex];

				Internal::SystemBase& systemInstance = module.GetSystemInstance(systemIndex).As<Internal::SystemBase>();
				Internal::SystemConfiguration& config = configs.emplace_back(&s_Data.TemporaryQueryComponents, defaultGroup.value());

				systemInstance.Configure(config);

				if (!systems.IsGroupIdValid(config.Group))
				{
					Grapple_CORE_ERROR("System '{0}' cannot be registered because the specified system group id '{1}' is not valid",
						systemInfo->Name, config.Group);
					continue;
				}

				systems.AddSystemToGroup(systemInfo->Id, config.Group, &config.GetExecutionOrder());

				Query query = s_Data.CurrentWorld->GetRegistry().CreateQuery(ComponentSet(
					s_Data.TemporaryQueryComponents.data(),
					s_Data.TemporaryQueryComponents.size()));

				systems.SetSystemQuery(systemInfo->Id, query);
			}
		}

		size_t configIndex = 0;
		for (ScriptingModuleData& module : s_Data.Modules)
		{
			const auto& registeredSystems = *module.Config.RegisteredSystems;
			for (size_t systemIndex = 0; systemIndex < registeredSystems.size(); systemIndex++)
			{
				const Internal::SystemInfo* systemInfo = registeredSystems[systemIndex];
				const Internal::SystemConfiguration& config = configs[configIndex++];
				systems.AddSystemExecutionSettings(systemInfo->Id, &config.GetExecutionOrder());
			}
		}
	}

	std::optional<const Internal::ScriptingType*> ScriptingEngine::FindType(std::string_view name)
	{
		auto it = s_Data.TypeNameToIndex.find(std::string(name));
		if (it == s_Data.TypeNameToIndex.end())
			return {};
		return GetScriptingType(it->second);
	}

	std::optional<const Internal::ScriptingType*> ScriptingEngine::FindSystemType(uint32_t systemIndex)
	{
		auto it = s_Data.SystemIndexToInstance.find(systemIndex);
		if (it == s_Data.SystemIndexToInstance.end())
			return {};

		ScriptingItemIndex instanceIndex = it->second;
		return GetScriptingType(s_Data
			.Modules[instanceIndex.ModuleIndex]
			.ScriptingInstances[instanceIndex.IndexInModule].Type);
	}

	std::optional<Internal::SystemBase*> ScriptingEngine::FindSystemByName(std::string_view name)
	{
		auto it = s_Data.SystemNameToInstance.find(name);
		if (it == s_Data.SystemNameToInstance.end())
			return {};

		ScriptingItemIndex index = it->second;
		void* instance = s_Data.Modules[index.ModuleIndex].ScriptingInstances[index.IndexInModule].Instance;
		return (Internal::SystemBase*)instance;
	}

	std::optional<uint8_t*> ScriptingEngine::FindSystemInstance(uint32_t systemIndex)
	{
		auto it = s_Data.SystemIndexToInstance.find(systemIndex);
		if (it == s_Data.SystemIndexToInstance.end())
			return {};

		ScriptingItemIndex instanceIndex = s_Data.SystemIndexToInstance[systemIndex];
		return (uint8_t*) s_Data.Modules[instanceIndex.ModuleIndex].ScriptingInstances[instanceIndex.IndexInModule].Instance;
	}

	std::optional<const Internal::ScriptingType*> ScriptingEngine::FindComponentType(ComponentId id)
	{
		auto it = s_Data.ComponentIdToTypeIndex.find(id);
		if (it != s_Data.ComponentIdToTypeIndex.end())
			return (*s_Data.Modules[it->second.ModuleIndex].Config.RegisteredTypes)[it->second.IndexInModule];
		
		return {};
	}
}