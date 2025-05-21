#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/Sprite.h"

namespace Grapple
{
    class SpriteEditor
    {
    public:
        SpriteEditor()
            : m_Sprite(nullptr) {}

        SpriteEditor(const Ref<Sprite>& sprite)
            : m_Sprite(sprite) {}

        bool OnImGuiRenderer();
    private:
        Ref<Sprite> m_Sprite;

        float m_Zoom = 1.0f;
    };
}
