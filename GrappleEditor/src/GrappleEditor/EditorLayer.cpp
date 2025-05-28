#include "EditorLayer.h"

#include "GrappleCore/Profiler/Profiler.h"
#include "GrappleCore/Serialization/SerializationStream.h"

#include "Grapple.h"
#include "Grapple/Core/Application.h"
#include "Grapple/Core/Time.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Font.h"

#include "Grapple/Renderer/PostProcessing/ToneMapping.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Project/Project.h"

#include "Grapple/Scripting/ScriptingEngine.h"
#include "Grapple/Input/InputManager.h"

#include "GrapplePlatform/Platform.h"
#include "GrapplePlatform/Events.h"

#include "GrappleEditor/Serialization/SceneSerializer.h"
#include "GrappleEditor/Serialization/YAMLSerialization.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"
#include "GrappleEditor/AssetManager/EditorShaderCache.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"
#include "GrappleEditor/UI/EditorTitleBar.h"
#include "GrappleEditor/UI/ProjectSettingsWindow.h"
#include "GrappleEditor/UI/ECS/ECSInspector.h"
#include "GrappleEditor/UI/PrefabEditor.h"
#include "GrappleEditor/UI/SceneViewportWindow.h"
#include "GrappleEditor/UI/SerializablePropertyRenderer.h"

#include "GrappleEditor/Scripting/BuildSystem/BuildSystem.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <string_view>

namespace Grapple
{
    EditorLayer* EditorLayer::s_Instance = nullptr;

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), 
        m_EditedSceneHandle(NULL_ASSET_HANDLE), 
        m_PropertiesWindow(m_AssetManagerWindow),
        m_PlaymodePaused(false),
        m_Mode(EditorMode::Edit),
        m_ProjectFilesWacher(nullptr)
    {
        s_Instance = this;

        Project::OnProjectOpen.Bind(Grapple_BIND_EVENT_CALLBACK(OnOpenProject));
        Project::OnUnloadActiveProject.Bind([this]()
        {
            m_ProjectFilesWacher.reset();
            if (Scene::GetActive() == nullptr)
                return;

            Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());

            Scene::GetActive()->UninitializePostProcessing();

            assetManager->UnloadAsset(Scene::GetActive()->Handle);

            ScriptingEngine::UnloadAllModules();
            m_ECSContext.Clear();

            Scene::SetActive(nullptr);
        });
    }

    EditorLayer::~EditorLayer()
    {
        s_Instance = nullptr;
    }

    void EditorLayer::OnAttach()
    {
        ShaderCacheManager::SetInstance(CreateScope<EditorShaderCache>());
        EditorGUI::Initialize();

        m_ImGuiLayer = ImGuiLayer::Create();
        m_PropertiesWindow.OnAttach();

        m_ImGuiLayer->OnAttach();

        Ref<Font> defaultFont = CreateRef<Font>("assets/Fonts/Roboto/Roboto-Regular.ttf");
        Font::SetDefault(defaultFont);

        AssetManager::Intialize(CreateRef<EditorAssetManager>());

        m_AssetManagerWindow.SetOpenAction(AssetType::Scene, [this](AssetHandle handle)
        {
            OpenScene(handle);
        });

        m_GameWindow = CreateRef<ViewportWindow>("Game");
        m_ViewportWindows.emplace_back(CreateRef<SceneViewportWindow>(m_Camera));
        m_ViewportWindows.emplace_back(m_GameWindow);

        EditorCameraSettings& settings = m_Camera.GetSettings();
        settings.FOV = 60.0f;
        settings.Near = 0.1f;
        settings.Far = 1000.0f;
        settings.RotationSpeed = 1.0f;

        if (Application::GetInstance().GetCommandLineArguments().ArgumentsCount >= 2)
        {
            std::filesystem::path projectPath = Application::GetInstance().GetCommandLineArguments()[1];
            Project::OpenProject(projectPath);
        }
        else
        {
            std::optional<std::filesystem::path> projectPath = Platform::ShowOpenFileDialog(
                L"Grapple Project (*.Grappleproj)\0*.Grappleproj\0",
                Application::GetInstance().GetWindow());

            if (projectPath.has_value())
                Project::OpenProject(projectPath.value());
        }

        m_PrefabEditor = CreateRef<PrefabEditor>(m_ECSContext);
        m_SpriteEditor = CreateRef<SpriteEditor>();

        m_AssetEditorWindows.push_back(m_PrefabEditor);
        m_AssetEditorWindows.push_back(m_SpriteEditor);

        m_AssetManagerWindow.SetOpenAction(AssetType::Prefab, [this](AssetHandle handle)
        {
            m_PrefabEditor->Open(handle);
        });

        m_AssetManagerWindow.SetOpenAction(AssetType::Sprite, [this](AssetHandle handle)
        {
            m_SpriteEditor->Open(handle);
        });

        for (auto& viewportWindow : m_ViewportWindows)
            viewportWindow->OnAttach();

        m_PrefabEditor->OnAttach();
    }

    void EditorLayer::OnDetach()
    {
        m_AssetManagerWindow.ClearOpenActions();
        m_AssetEditorWindows.clear();

        m_ImGuiLayer->OnDetach();

        if (m_Mode == EditorMode::Play)
            ExitPlayMode();

        if (Scene::GetActive() != nullptr && AssetManager::IsAssetHandleValid(Scene::GetActive()->Handle))
            As<EditorAssetManager>(AssetManager::GetInstance())->UnloadAsset(Scene::GetActive()->Handle);

        m_PrefabEditor->OnDetach();
        m_PrefabEditor = nullptr;

        Scene::SetActive(nullptr);
    }

    void EditorLayer::OnUpdate(float deltaTime)
    {
        Grapple_PROFILE_FUNCTION();

        if (m_Mode == EditorMode::Play)
        {
            if (m_GameWindow->HasFocusChanged())
            {
                if (!m_GameWindow->IsFocused())
                    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
                else
                    m_UpdateCursorModeNextFrame = true;
            }

            if (m_UpdateCursorModeNextFrame && !m_PlaymodePaused)
            {
                Application::GetInstance().GetWindow()->SetCursorMode(InputManager::GetCursorMode());
                m_UpdateCursorModeNextFrame = false;

                switch (InputManager::GetCursorMode())
                {
                case CursorMode::Hidden:
                case CursorMode::Disabled:
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
                    break;
                }
            }
        }

        Renderer2D::ResetStats();
        Renderer::ClearStatistics();

        RenderCommand::SetClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0);

        Renderer::SetMainViewport(m_GameWindow->GetViewport());
        InputManager::SetMousePositionOffset(-m_GameWindow->GetViewport().GetPosition());

        {
            Grapple_PROFILE_SCOPE("Scene Runtime Update");

            Ref<Scene> activeScene = Scene::GetActive();

            if (m_Mode == EditorMode::Play && !m_PlaymodePaused)
                activeScene->OnUpdateRuntime();
            else if (m_Mode == EditorMode::Edit && activeScene)
                activeScene->OnUpdateEditor();
        }

        if (RendererAPI::GetAPI() != RendererAPI::API::Vulkan)
        {
            Grapple_PROFILE_SCOPE("Viewport Render");
            for (auto& viewport : m_ViewportWindows)
            {
                viewport->OnRenderViewport();
            }
        }

        if (m_ProjectFilesWacher)
        {
            m_ProjectFilesWacher->Update();
            FileChangeEvent changes;

            Ref<EditorAssetManager> editorAssetManager = As<EditorAssetManager>(AssetManager::GetInstance());

            bool shouldRebuildAssetTree = false;
            while (true)
            {
                auto result = m_ProjectFilesWacher->TryGetNextEvent(changes);
                if (result != FileWatcher::Result::Ok)
                    break;

                switch (changes.Action)
                {
                case FileChangeEvent::ActionType::Created:
                    shouldRebuildAssetTree = true;
                    break;
                case FileChangeEvent::ActionType::Deleted:
                    shouldRebuildAssetTree = true;
                    break;
                case FileChangeEvent::ActionType::Renamed:
                    shouldRebuildAssetTree = true;
                    break;
                case FileChangeEvent::ActionType::Modified:
                    std::filesystem::path absoluteFilePath = Project::GetActive()->Location / changes.FilePath;
                    std::optional<AssetHandle> handle = editorAssetManager->FindAssetByPath(absoluteFilePath);

                    bool isValid = handle.has_value() && AssetManager::IsAssetHandleValid(handle.value());
                    if (isValid && AssetManager::IsAssetLoaded(handle.value_or(NULL_ASSET_HANDLE)))
                    {
                        const AssetMetadata* metadata = AssetManager::GetAssetMetadata(handle.value());
                        Grapple_CORE_ASSERT(metadata);

                        if (metadata->Type == AssetType::Shader)
                        {
                            if (Application::GetInstance().GetWindow()->GetProperties().IsFocused)
                                editorAssetManager->ReloadAsset(*handle);
                            else
                                m_AssetReloadQueue.emplace(*handle);
                        }
                    }

                    break;
                }
            }

            if (shouldRebuildAssetTree)
            {
                m_AssetManagerWindow.RebuildAssetTree();
            }
        }
    }

    void EditorLayer::OnEvent(Event& event)
    {
        Grapple_PROFILE_FUNCTION();
        for (Ref<ViewportWindow>& window : m_ViewportWindows)
            window->OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& e) -> bool
        {
            if (e.GetKeyCode() == KeyCode::Escape)
                Application::GetInstance().GetWindow()->SetCursorMode(CursorMode::Normal);
            return false;
        });

        dispatcher.Dispatch<WindowFocusEvent>([this](WindowFocusEvent& e) -> bool
        {
            if (e.IsFocused())
            {
                Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
                for (AssetHandle handle : m_AssetReloadQueue)
                    assetManager->ReloadAsset(handle);

                m_AssetReloadQueue.clear();
                m_AssetManagerWindow.RebuildAssetTree();
            }
            return false;
        });

        if (!event.Handled)
            m_PrefabEditor->OnEvent(event);
        
        // InputManager only works with Game viewport,
        // so the events should only be processed when the Game window is focused
        if (!event.Handled && m_GameWindow->IsFocused())
            InputManager::ProcessEvent(event);
    }

    void EditorLayer::OnImGUIRender()
    {
        Grapple_PROFILE_FUNCTION();
        m_ImGuiLayer->Begin();

        static bool fullscreen = true;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        if (fullscreen)
        {
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        static bool open = true;
        ImGui::Begin("DockSpace", &open, windowFlags);

        ImGuiID dockspaceId = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

        m_TitleBar.OnRenderImGui();

        {
            ImGui::Begin("Shadows");

            static int32_t s_CascadeIndex = 0;

            ShadowSettings& settings = Renderer::GetShadowSettings();

            ImGui::SliderInt("Cascade Index", &s_CascadeIndex, 0, settings.Cascades - 1, "%d", ImGuiSliderFlags_AlwaysClamp);

            auto shadows = Renderer::GetShadowsRenderTarget(s_CascadeIndex);
            if (shadows)
                ImGui::Image(shadows->GetColorAttachmentRendererId(0), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::End();
        }

        {
            ImGui::Begin("Renderer");
            const auto& stats = Renderer::GetStatistics();

            ImGui::Text("Frame time %f ms", Time::GetDeltaTime() * 1000.0f);
            ImGui::Text("FPS %f", 1.0f / Time::GetDeltaTime());
            ImGui::Text("Shadow Pass %f ms", stats.ShadowPassTime);
            ImGui::Text("Geometry Pass %f ms", stats.GeometryPassTime);

            Ref<Window> window = Application::GetInstance().GetWindow();

            bool vsync = window->GetProperties().VSyncEnabled;
            if (ImGui::Checkbox("VSync", &vsync))
                window->SetVSync(vsync);

            ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_ClearColor), ImGuiColorEditFlags_Float);

            if (ImGui::CollapsingHeader("Renderer 2D"))
            {
                const auto& stats = Renderer2D::GetStats();
                ImGui::Text("Quads %d", stats.QuadsCount);
                ImGui::Text("Draw Calls %d", stats.DrawCalls);
                ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
            }

            if (ImGui::CollapsingHeader("Renderer"))
            {
                ImGui::Text("Submitted: %d Culled: %d", stats.ObjectsSubmitted, stats.ObjectsCulled);
                ImGui::Text("Draw calls (Saved by instancing: %d): %d", stats.DrawCallsSavedByInstancing, stats.DrawCallsCount);
            }

            if (ImGui::TreeNodeEx("Post Processing", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                auto& postProcessing = Scene::GetActive()->GetPostProcessingManager();

                EditorGUI::ObjectField(
                    Grapple_SERIALIZATION_DESCRIPTOR_OF(ToneMapping),
                    postProcessing.ToneMappingPass.get());
                EditorGUI::ObjectField(
                    Grapple_SERIALIZATION_DESCRIPTOR_OF(Vignette),
                    postProcessing.VignettePass.get());
                EditorGUI::ObjectField(
                    Grapple_SERIALIZATION_DESCRIPTOR_OF(SSAO),
                    postProcessing.SSAOPass.get());
                EditorGUI::ObjectField(
                    Grapple_SERIALIZATION_DESCRIPTOR_OF(AtmospherePass),
                    postProcessing.Atmosphere.get());

                ImGui::TreePop();
            }

            ImGui::End();
        }

        {
            Grapple_PROFILE_SCOPE("ViewportWindows ImGui");
            for (auto& viewport : m_ViewportWindows)
                viewport->OnRenderImGui();
        }

        {
            Grapple_PROFILE_SCOPE("EditorWindowsUpdate");
            
            ProjectSettingsWindow::OnRenderImGui();
            m_SceneWindow.OnImGuiRender();
            m_PropertiesWindow.OnImGuiRender();
            m_AssetManagerWindow.OnImGuiRender();
            m_QuickSearch.OnImGuiRender();
            m_ProfilerWindow.OnImGuiRender();

            ECSInspector::GetInstance().OnImGuiRender();

            for (auto& window : m_AssetEditorWindows)
                window->OnUpdate();
        }

        ImGui::End();
        m_ImGuiLayer->End();
        m_ImGuiLayer->RenderCurrentWindow();
        m_ImGuiLayer->UpdateWindows();
    }

    void EditorLayer::UpdateWindowTitle()
    {
        if (Project::GetActive() != nullptr)
        {
            std::string name = fmt::format("Grapple Editor - {0} - {1}", Project::GetActive()->Name, Project::GetActive()->Location.generic_string());
            Application::GetInstance().GetWindow()->SetTitle(name);
        }
    }

    void EditorLayer::OnOpenProject()
    {
        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());

        assetManager->Reinitialize();

        UpdateWindowTitle();
        m_AssetManagerWindow.RebuildAssetTree();

        AssetHandle startScene = Project::GetActive()->StartScene;

        ScriptingEngine::LoadModules();
        m_ECSContext.Components.RegisterComponents();

        OpenScene(startScene);
        assetManager->ReloadPrefabs(); // HACK: component ids have changed after reregistering components, so reload prefabs

        m_ProjectFilesWacher.reset(FileWatcher::Create(Project::GetActive()->Location, EventsMask::FileName | EventsMask::DirectoryName | EventsMask::LastWrite));
    }

    void EditorLayer::SaveActiveScene()
    {
        Grapple_CORE_ASSERT(Scene::GetActive());
        if (AssetManager::IsAssetHandleValid(Scene::GetActive()->Handle))
            SceneSerializer::Serialize(Scene::GetActive(), m_Camera, m_SceneViewSettings);
        else
            SaveActiveSceneAs();
    }

    void EditorLayer::SaveActiveSceneAs()
    {
        std::optional<std::filesystem::path> scenePath = Platform::ShowSaveFileDialog(
            L"Grapple Scene (*.Grapple)\0*.Grapple\0",
            Application::GetInstance().GetWindow());

        if (scenePath.has_value())
        {
            std::filesystem::path& path = scenePath.value();
            if (!path.has_extension())
                path.replace_extension(".Grapple");

            SceneSerializer::Serialize(Scene::GetActive(), path, m_Camera, m_SceneViewSettings);
            AssetHandle handle = As<EditorAssetManager>(AssetManager::GetInstance())->ImportAsset(path);
            OpenScene(handle);
        }
    }

    void EditorLayer::OpenScene(AssetHandle handle)
    {
        if (AssetManager::IsAssetHandleValid(handle))
        {
            Ref<Scene> active = Scene::GetActive();

            Ref<EditorAssetManager> editorAssetManager = As<EditorAssetManager>(AssetManager::GetInstance());

            if (active != nullptr && AssetManager::IsAssetHandleValid(active->Handle))
                editorAssetManager->UnloadAsset(active->Handle);

            active = nullptr;
            Scene::SetActive(nullptr);

            active = AssetManager::GetAsset<Scene>(handle);
            Scene::SetActive(active);

            active->InitializeRuntime();
            active->InitializePostProcessing();

            m_EditedSceneHandle = handle;
        }
    }

    void EditorLayer::CreateNewScene()
    {
        Ref<Scene> active = Scene::GetActive();

        if (active != nullptr)
        {
            active->UninitializePostProcessing();
            Ref<EditorAssetManager> editorAssetManager = As<EditorAssetManager>(AssetManager::GetInstance());

            if (active != nullptr && AssetManager::IsAssetHandleValid(active->Handle))
                editorAssetManager->UnloadAsset(active->Handle);
        }

        active = nullptr;

        active = CreateRef<Scene>(m_ECSContext);
        active->Initialize();
        active->InitializeRuntime();
        active->InitializePostProcessing();
        Scene::SetActive(active);

        m_EditedSceneHandle = NULL_ASSET_HANDLE;
    }

    void EditorLayer::EnterPlayMode()
    {
        Grapple_CORE_ASSERT(m_Mode == EditorMode::Edit);

        m_GameWindow->RequestFocus();
        m_UpdateCursorModeNextFrame = true;

        Ref<Scene> active = Scene::GetActive();

        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
        std::filesystem::path activeScenePath = assetManager->GetAssetMetadata(active->Handle)->Path;

        Scene::GetActive()->UninitializePostProcessing();
        SaveActiveScene();

        m_PlaymodePaused = false;

        Scene::SetActive(nullptr);
        assetManager->UnloadAsset(active->Handle);
        active = nullptr;

        Ref<Scene> playModeScene = CreateRef<Scene>(m_ECSContext);
        SceneSerializer::Deserialize(playModeScene, activeScenePath, m_Camera, m_SceneViewSettings);

        Scene::SetActive(playModeScene);
        m_Mode = EditorMode::Play;

        playModeScene->InitializeRuntime();
        playModeScene->InitializePostProcessing();
        Scene::GetActive()->OnRuntimeStart();
    }

    void EditorLayer::ExitPlayMode()
    {
        Grapple_CORE_ASSERT(m_Mode == EditorMode::Play);
        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());

        Scene::GetActive()->OnRuntimeEnd();
        Scene::GetActive()->UninitializePostProcessing();

        Scene::SetActive(nullptr);

        Ref<Scene> editorScene = AssetManager::GetAsset<Scene>(m_EditedSceneHandle);
        editorScene->InitializeRuntime();
        editorScene->InitializePostProcessing();

        Scene::SetActive(editorScene);
        m_Mode = EditorMode::Edit;

        InputManager::SetCursorMode(CursorMode::Normal);
    }

    void EditorLayer::ReloadScriptingModules()
    {
        Grapple_CORE_ASSERT(!Platform::IsDebuggerAttached());
        Grapple_CORE_ASSERT(m_Mode == EditorMode::Edit);

        Ref<Scene> active = Scene::GetActive();
        AssetHandle activeSceneHandle = active->Handle;

        Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
        std::filesystem::path activeScenePath = assetManager->GetAssetMetadata(active->Handle)->Path;
        SaveActiveScene();

        active->UninitializePostProcessing();

        Scene::SetActive(nullptr);
        assetManager->UnloadAsset(active->Handle);
        active = nullptr;

        ScriptingEngine::UnloadAllModules();

        Grapple_CORE_INFO("Compiling...");
        BuildSystem::BuildModules();
        Grapple_CORE_INFO("Linking...");
        BuildSystem::LinkModules();

        ScriptingEngine::LoadModules();
        m_ECSContext.Components.ReregisterComponents();

        active = CreateRef<Scene>(m_ECSContext);
        active->Handle = activeSceneHandle;
        SceneSerializer::Deserialize(active, activeScenePath, m_Camera, m_SceneViewSettings);

        ScriptingEngine::SetCurrentECSWorld(active->GetECSWorld());
        ScriptingEngine::RegisterSystems();
        active->InitializeRuntime();
        active->InitializePostProcessing();
        Scene::SetActive(active);

        assetManager->ReloadPrefabs();
    }

    EditorLayer& EditorLayer::GetInstance()
    {
        Grapple_CORE_ASSERT(s_Instance != nullptr, "Invalid EditorLayer instance");
        return *s_Instance;
    }
}
