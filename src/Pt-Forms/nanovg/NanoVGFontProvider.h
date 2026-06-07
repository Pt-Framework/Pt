/* Copyright (C) 2026 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_NanoVGFontProvider_h
#define Pt_Forms_NanoVGFontProvider_h

#include <Pt/Gfx/Font.h>

#include <string>
#include <vector>

struct NVGcontext;

namespace Pt {

namespace Forms {

// Resolves Gfx::Font requests to nanovg font handles. The provider registers
// the embedded default fonts and all files known to Gfx::FontRegistry with the
// shared nanovg context and selects the closest match for a request using a
// score over weight, slant and stretch.
//
// Variable fonts are matched to their nearest static instance only. Per axis
// coordinates are not applied (v1 limitation).
class NanoVGFontProvider
{
    public:
        explicit NanoVGFontProvider(NVGcontext* nvg);

        // Returns the nanovg font handle for the request, or -1 if no font is
        // available.
        int fontFace(const Gfx::Font& font);

        // Returns the ratio between line height and ascender for the given
        // nanovg font handle. The nanovg/FreeType pixel size is computed as
        // font.size() * sizeScale() so that the ascender height matches the
        // requested point size, mirroring the raster renderer.
        float sizeScale(int handle) const;

        // Returns face->ascender / face->units_per_EM for the given handle.
        // Multiply by the nanovg font size to get the true typographic
        // ascender in canvas units, overriding the lineGap-adjusted value
        // that fontstash reports via fonsVertMetrics.
        float ascenderRatio(int handle) const;

        // Returns the cap height as a ratio of the ascender, read from the
        // OS/2 table sCapHeight field. Returns 0.7 when not available.
        float capHeightRatio(int handle) const;

        // Returns the x-height as a ratio of the ascender, read from the
        // OS/2 table sxHeight field. Returns 0.54 when not available.
        float xHeightRatio(int handle) const;

    private:
        struct FaceEntry
        {
            std::string family;
            std::string styleName;
            int         weight;   // 100..900
            int         slant;    // 0 = upright, 1 = italic/oblique
            int         stretch;  // 1..9
            int         handle;          // nanovg font handle
            float       scale;           // line height / ascender
            float       ascenderRatio;   // face->ascender / face->units_per_EM
            float       capHeightRatio;  // cap height / ascender (OS/2 sCapHeight)
            float       xHeightRatio;    // x-height / ascender   (OS/2 sxHeight)
        };

        void registerEmbedded();

        void loadFile(const std::string& path);

        const FaceEntry* findBestMatch(const std::string& family,
                                       const std::string& styleName,
                                       int weight, int slant, int stretch) const;

        static std::string categoryDefaultFamily(Gfx::Font::Category category);

        static int matchScore(int weight, int slant, int stretch,
                              const FaceEntry& face);

    private:
        NVGcontext*            _nvg;
        std::vector<FaceEntry> _faces;
        int                    _defaultRegular;
        int                    _defaultBold;
        int                    _defaultItalic;
        int                    _defaultBoldItalic;
};

} // namespace

} // namespace

#endif
