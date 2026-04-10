/* Copyright (C) 2024 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_FORMS_COCOA_COCOAFONTPROVIDER_H
#define PT_FORMS_COCOA_COCOAFONTPROVIDER_H

#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/FontProvider.h>
#include <Pt/System/Path.h>

#include <CoreText/CoreText.h>

#include <string>
#include <vector>

namespace Pt {

namespace Forms {

class CocoaFontProvider : public Gfx::FontProvider
{
    public:
        struct Init
        {
            Init()
            { CocoaFontProvider::instance(); }
        };

        static CocoaFontProvider& instance();

        const std::string& defaultFont() const;

        void setDefaultFont(const std::string& font);

        CTFontRef lookupFont(const Gfx::Font& font) const;

        std::vector<std::string> fontFamilies() const;

        std::vector<Gfx::FontFace> fontFaces(const std::string& family) const;

        ~CocoaFontProvider();

    private:
        CocoaFontProvider();

        CocoaFontProvider(const CocoaFontProvider&) = delete;

        CocoaFontProvider& operator=(const CocoaFontProvider&) = delete;

        virtual void onAddFont(const System::Path& path) override;

        virtual void onRemoveFont(const System::Path& path) override;

    private:
        static std::string toUtf8String(CFStringRef text);

        static float ctFontWeight(Gfx::Font::Weight weight);

        static float ctFontSlant(Gfx::Font::Slant slant);

        static Gfx::FontFace::Weight fontWeightFromCtWeight(float weight);

        static Gfx::FontFace::Slant fontSlantFromCtTraits(float slant,
                                                          CTFontSymbolicTraits symbolicTraits);

        static Gfx::FontFace makeFontFace(const std::string& family,
                                          const std::string& style,
                                          CTFontDescriptorRef descriptor);

    private:
        static bool registerFontFile(const System::Path& path);

        static void unregisterFontFile(const System::Path& path);

    private:
        std::vector<System::Path> _files;
        std::string _defaultFont;
};


static CocoaFontProvider::Init initCocoaFontProvider;

} // namespace Forms

} // namespace Pt

#endif