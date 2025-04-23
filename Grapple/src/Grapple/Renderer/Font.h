#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/Renderer/Texture.h"

#define MSDF_ATLAS_PUBLIC
#include <msdf-atlas-gen.h>

#include <filesystem>
#include <vector>

namespace Grapple
{
	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry Geometry;
	};

	class Grapple_API Font
	{
	public:
		Font(const std::filesystem::path& path);

		Ref<Texture> GetAtlas() const { return m_FontAtlas; }
		inline const MSDFData& GetData() const { return m_Data; }

		static Ref<Font> GetDefault();
		static void SetDefault(const Ref<Font>& font);
	private:
		MSDFData m_Data;
		Ref<Texture> m_FontAtlas;
	};
}