#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/Renderer/Texture.h"

#include <filesystem>
#include <vector>

namespace Grapple
{
	struct MSDFData;

	class Grapple_API Font
	{
	public:
		Font(const std::filesystem::path& path);
		~Font();

		Ref<Texture> GetAtlas() const { return m_FontAtlas; }
	private:
		MSDFData* m_Data = nullptr;
		Ref<Texture> m_FontAtlas;
	};
}