/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#ifndef Pt_Forms_GdiFontProvider_h
#define Pt_Forms_GdiFontProvider_h

#include "win32.h"

#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/FontProvider.h>
#include <Pt/System/Path.h>

#include <string>
#include <vector>

namespace Pt {

namespace Forms {

class GdiFontProvider : public Gfx::FontProvider
{
    public:
        struct Init
        {
            Init()
            { GdiFontProvider::instance(); }
        };

        static GdiFontProvider& instance();

        const std::string& defaultFont() const;

        void setDefaultFont(const std::string& font);

        HFONT lookupFont(const Gfx::Font& font) const;

        std::vector<std::string> fontFamilies() const;

        std::vector<Gfx::FontFace> fontFaces(const std::string& family) const;

        ~GdiFontProvider();

    private:
        GdiFontProvider();

        GdiFontProvider(const GdiFontProvider&) = delete;

        GdiFontProvider& operator=(const GdiFontProvider&) = delete;

        virtual void onAddFont(const System::Path& path) override;

        virtual void onRemoveFont(const System::Path& path) override;

    private:
        struct FontFamilyList
        {
            std::vector<std::string> families;
        };

        static Gfx::FontFace::Weight fontWeightFromLogFontWeight(LONG weight);

        static Gfx::FontFace::Slant fontSlantFromLogFontItalic(BYTE italic);

        static int CALLBACK enumFontFamExProc(ENUMLOGFONTEX* logFont, NEWTEXTMETRICEX* physFont, DWORD type, LPARAM param);

        static int CALLBACK enumFontFamilyNamesExProc(ENUMLOGFONTEX* logFont, NEWTEXTMETRICEX* physFont, DWORD type, LPARAM param);

    private:
        static bool registerFontFile(const System::Path& path);

        static void unregisterFontFile(const System::Path& path);

    private:
        std::vector<System::Path> _files;
        std::string _defaultFont;
};


static GdiFontProvider::Init initGdiFontProvider;

} // namespace Forms

} // namespace Pt

#endif // Pt_Forms_GdiFontProvider_h
