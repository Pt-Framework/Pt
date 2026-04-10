/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

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

#ifndef PT_GFX_FONT_H
#define PT_GFX_FONT_H

#include <Pt/Gfx/FontBase.h>
#include <Pt/SmartPtr.h>

#include <cstddef>
#include <string>

namespace Pt {

namespace Gfx {

class FontData;

class PT_GFX_API Font : public FontBase
{
    public:
        //! @brief Default constructor.
        Font();

        //! @brief Construct a font with an exact style name hint.
        Font(const std::string& family, std::size_t size,
             const std::string& styleName,
             Weight weight = Weight::Normal,
             Slant slant = Slant::Normal);

        //! @brief Construct a font with explicit weight and slant.
        Font(const std::string& family, std::size_t size,
             Weight weight = Weight::Normal,
             Slant slant = Slant::Normal);

        //! @brief Returns the family of the font.
        const std::string& family() const;

        //! @brief Returns the size of the font
        std::size_t size() const;

        //! @brief Returns the optional exact style name hint.
        const std::string& styleName() const;

        //! @brief Returns true if an exact style name hint is set.
        bool hasStyleName() const;

        //! @brief Returns the weight of the font request.
        Weight weight() const;

        //! @brief Returns the slant of the font request.
        Slant slant() const;

    private:
        SmartPtr<FontData> _fontData;
};


inline bool operator==(const Font& a, const Font& b)
{
    return a.family() == b.family() &&
           a.weight() == b.weight() &&
           a.slant()  == b.slant()  &&
           a.size()   == b.size()   &&
           a.styleName() == b.styleName();
}


inline bool operator!=(const Font& a, const Font& b)
{
    return a.family() != b.family() ||
           a.weight() != b.weight() ||
           a.slant()  != b.slant()  ||
           a.size()   != b.size()   ||
           a.styleName() != b.styleName();
}


inline bool operator<(const Font& a, const Font& b)
{
    if(a.family() != b.family())
        return a.family() < b.family();

    if(a.weight() != b.weight())
        return a.weight() < b.weight();

    if(a.slant() != b.slant())
        return a.slant() < b.slant();

    if(a.size() != b.size())
        return a.size() < b.size();

    return a.styleName() < b.styleName();
}


class FontData
{
    public:
        FontData()
        : _family()
        , _size(0)
        , _styleName()
        , _weight(FontBase::Weight::Normal)
        , _slant(FontBase::Slant::Normal)
        {
        }

        FontData(const std::string& family, std::size_t size,
                 FontBase::Weight weight = FontBase::Weight::Normal,
                 FontBase::Slant slant = FontBase::Slant::Normal,
                 const std::string& styleName = std::string())
        : _family(family)
        , _size(size)
        , _styleName(styleName)
        , _weight(weight)
        , _slant(slant)
        {
        }

        FontData(const std::string& family, const FontData& font)
        : _family(family)
        , _size(font._size)
        , _styleName(font._styleName)
        , _weight(font._weight)
        , _slant(font._slant)
        {
        }

        const std::string& family() const
        {
            return _family;
        }

        std::size_t size() const
        {
            return _size;
        }

        const std::string& styleName() const
        {
            return _styleName;
        }

        bool hasStyleName() const
        {
            return ! _styleName.empty();
        }

        FontBase::Weight weight() const
        {
            return _weight;
        }

        FontBase::Slant slant() const
        {
            return _slant;
        }

    private:
        std::string _family;
        std::size_t _size;
        std::string _styleName;
        FontBase::Weight _weight;
        FontBase::Slant _slant;
};

} //namespace

} //namespace

#endif
