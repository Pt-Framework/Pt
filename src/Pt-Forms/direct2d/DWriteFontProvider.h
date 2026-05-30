/* Copyright (C) 2015-2026 Marc Boris Duerner

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

#ifndef Pt_Forms_DWriteFontProvider_h
#define Pt_Forms_DWriteFontProvider_h

#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/FontProvider.h>
#include <Pt/System/Path.h>

#include <string>
#include <vector>

#include <dwrite.h>

namespace Pt {

namespace Forms {

class DWriteFontProvider : public Gfx::FontProvider
{
    public:
        static DWriteFontProvider& instance();

        const std::string& defaultFont() const;

        void setDefaultFont(const std::string& font);

        std::vector<std::string> fontFamilies() const;

        std::vector<Gfx::FontFace> fontFaces(const std::string& family) const;

        ~DWriteFontProvider();

    private:
        DWriteFontProvider();

        DWriteFontProvider(const DWriteFontProvider&) = delete;

        DWriteFontProvider& operator=(const DWriteFontProvider&) = delete;

        virtual void onAddFont(const System::Path& path) override;

        virtual void onRemoveFont(const System::Path& path) override;

    private:
        std::string _defaultFont;
};

} // namespace

} // namespace

#endif
