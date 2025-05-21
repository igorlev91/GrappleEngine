#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/Sprite.h"

#include "Grapple/Math/Math.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"
#include "GrappleEditor/UI/AssetEditor.h"

namespace Grapple
{
    class SpriteEditor : public AssetEditor
    {
    public:
        enum class SelectionRectSide
        {
            None = 0,
            Top = 1,
            Right = 2,
            Bottom = 4,
            Left = 8,

            TopLeft = Top | Left,
            TopRight = Top | Right,
            BottomLeft = Bottom | Left,
            BottomRight = Bottom | Right,
        };

        void OnEvent(Event& event) override;
        void OnOpen(AssetHandle asset) override;
        void OnClose() override;
        void OnRenderImGui(bool& show) override;
    private:
        void RenderWindowContent();
        void RenderViewport();
        void RenderSidebar();
        void RenderSidebarContent();

        ImVec2 WindowToTextureSpace(ImVec2 windowSpace);
        ImVec2 TextureToWindowSpace(ImVec2 textureSpace);

        SelectionRectSide RenderSelectionRect(ImRect imageRect);
        bool RenderSelectionSide(const char* name, ImVec2 position, ImVec2 size);

        void ValidateSelectionRect();
        Math::Rect SelectionToUVRect();
    private:
        Ref<Sprite> m_Sprite = nullptr;
        SelectionRectSide m_ResizedSides = SelectionRectSide::None;

        float m_Zoom = 1.0f;
        float m_SelectionRectCornerSize = 8.0f;
        float m_SideBarWidth = 300.0f;

        bool m_CanResize;
        bool m_HasSelection = false;
        bool m_SelectionStarted = false;

        ImVec2 m_SelectionStart = ImVec2(0.0f, 0.0f);
        ImVec2 m_SelectionEnd = ImVec2(0.0f, 0.0f);
    };
}
