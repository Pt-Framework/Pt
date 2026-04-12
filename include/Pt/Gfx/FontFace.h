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

#include <Pt/Gfx/Font.h>
#include <Pt/SmartPtr.h>

#include <string>

namespace Pt {

namespace Gfx {

class FontFaceData;

class PT_GFX_API FontFace
{
    public:
        typedef Font::Weight Weight;
        typedef Font::Slant Slant;
        typedef Font::Stretch Stretch;

        FontFace();

        FontFace(const std::string& family,
                 Weight weight = Weight::Normal,
                 Slant slant = Slant::Normal,
                 Stretch stretch = Stretch::Normal,
                 const std::string& styleName = std::string());

        const std::string& family() const;

        /** @brief Returns the family of the font face.
            Alias for family().
        */
        const std::string& name() const;

        const std::string& styleName() const;

        const std::string& style() const;

        Weight weight() const;

        Slant slant() const;

        Stretch stretch() const;

    private:
        SmartPtr<FontFaceData> _faceData;
};


inline bool operator==(const FontFace& a, const FontFace& b)
{
    return a.family() == b.family() &&
           a.weight() == b.weight() &&
           a.slant() == b.slant() &&
           a.stretch() == b.stretch() &&
           a.styleName() == b.styleName();
}


inline bool operator!=(const FontFace& a, const FontFace& b)
{
    return a.family() != b.family() ||
           a.weight() != b.weight() ||
           a.slant() != b.slant() ||
           a.stretch() != b.stretch() ||
           a.styleName() != b.styleName();
}


inline bool operator<(const FontFace& a, const FontFace& b)
{
    if(a.family() != b.family())
        return a.family() < b.family();

    if(a.weight() != b.weight())
        return a.weight() < b.weight();

    if(a.slant() != b.slant())
        return a.slant() < b.slant();

    if(a.stretch() != b.stretch())
        return a.stretch() < b.stretch();

    return a.styleName() < b.styleName();
}


class FontFaceData
{
    public:
        FontFaceData()
        : _family()
        , _style()
        , _weight(Font::Weight::Normal)
        , _slant(Font::Slant::Normal)
        , _stretch(Font::Stretch::Normal)
        {
        }

        FontFaceData(const std::string& family,
                     Font::Weight weight = Font::Weight::Normal,
                     Font::Slant slant = Font::Slant::Normal,
                     Font::Stretch stretch = Font::Stretch::Normal,
                     const std::string& styleName = std::string())
        : _family(family)
        , _style(styleName)
        , _weight(weight)
        , _slant(slant)
        , _stretch(stretch)
        {
        }

        const std::string& family() const
        {
            return _family;
        }

        const std::string& styleName() const
        {
            return _style;
        }

        const std::string& style() const
        {
            return _style;
        }

        Font::Weight weight() const
        {
            return _weight;
        }

        Font::Slant slant() const
        {
            return _slant;
        }

        Font::Stretch stretch() const
        {
            return _stretch;
        }

    private:
        std::string _family;
        std::string _style;
        Font::Weight _weight;
        Font::Slant _slant;
        Font::Stretch _stretch;
};

} // namespace

} // namespace

#endif