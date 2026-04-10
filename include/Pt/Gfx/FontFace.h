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

#ifndef PT_GFX_FONTFACE_H
#define PT_GFX_FONTFACE_H

#include <Pt/Gfx/Api.h>
#include <Pt/SmartPtr.h>

#include <string>

namespace Pt {

namespace Gfx {

class FontFaceData;

class PT_GFX_API FontFace
{
    public:
        FontFace();

        FontFace(const std::string& name,
                 const std::string& style = std::string());

        const std::string& name() const;

        const std::string& style() const;

    private:
        SmartPtr<FontFaceData> _faceData;
};


inline bool operator==(const FontFace& a, const FontFace& b)
{
    return a.name() == b.name() && a.style() == b.style();
}


inline bool operator!=(const FontFace& a, const FontFace& b)
{
    return a.name() != b.name() || a.style() != b.style();
}


inline bool operator<(const FontFace& a, const FontFace& b)
{
    return a.name() == b.name() ? a.style() < b.style()
                                : a.name() < b.name();
}


class FontFaceData
{
    public:
        FontFaceData()
        { }

        FontFaceData(const std::string& name,
                     const std::string& style = std::string())
        : _name(name)
        , _style(style)
        { }

        const std::string& name() const
        {
            return _name;
        }

        const std::string& style() const
        {
            return _style;
        }

    private:
        std::string _name;
        std::string _style;
};

} // namespace

} // namespace

#endif