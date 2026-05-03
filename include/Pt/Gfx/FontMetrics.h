/* Copyright (C) 2015-2024 Marc Boris Duerner

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

#ifndef PT_GFX_FONTMETRICS_H
#define PT_GFX_FONTMETRICS_H

#include <Pt/Gfx/Api.h>

namespace Pt {

namespace Gfx {

/** @brief Metrics that describe a font face at a given size.
    @ingroup Drawing

    FontMetrics contains values such as ascent, descent and underline geometry
    for the font selected by a painter. These metrics can be used to align
    text, compute line spacing and position decorative lines.
*/
class PT_GFX_API FontMetrics
{
    public:
        /** @brief Constructs empty font metrics.
        */
        FontMetrics();

        /** @brief Returns the ascent above the baseline.
        */
        Float ascent() const
        {
            return _ascent;
        }

        /** @brief Sets the ascent.
        */
        void setAscent(Float n)
        {
            _ascent = n;
        }

        /** @brief Returns the descent below the baseline.
        */
        Float descent() const
        {
            return _descent;
        }

        /** @brief Sets the descent.
        */
        void setDescent(Float n)
        {
            _descent = n;
        }

        /** @brief Returns the font height (ascent + descent).
        */
        Float height() const
        {
            return _ascent + _descent;
        }

        /** @brief Returns the height of capital letters.
        */
        Float capHeight() const
        {
            return _capHeight;
        }

        /** @brief Sets the cap height.
        */
        void setCapHeight(Float n)
        {
            _capHeight = n;
        }

        /** @brief Returns the height of lowercase x.
        */
        Float xHeight() const
        {
            return _xHeight;
        }

        /** @brief Sets the x-height.
        */
        void setXHeight(Float n)
        {
            _xHeight = n;
        }

        /** @brief Returns the external leading between lines.
        */
        Float leading() const
        {
            return _leading;
        }

        /** @brief Sets the leading.
        */
        void setLeading(Float n)
        {
            _leading = n;
        }

        /** @brief Returns the line height (ascent + descent + leading).
        */
        Float lineHeight() const
        {
            return _ascent + _descent + _leading;
        }

        /** @brief Returns the underline position below the baseline.
        */
        Float underlinePos() const
        {
            return _underlinePos;
        }

        /** @brief Sets the underline position.
        */
        void setUnderlinePos(Float n)
        {
            _underlinePos = n;
        }

        /** @brief Returns the underline thickness.
        */
        Float underlineThickness() const
        {
            return _underlineThickness;
        }

        /** @brief Sets the underline thickness.
        */
        void setUnderlineThickness(Float n)
        {
            _underlineThickness = n;
        }

        /** @brief Returns the strikeout position above the baseline.
        */
        Float strikeoutPos() const
        {
            return _strikeoutPos;
        }

        /** @brief Sets the strikeout position.
        */
        void setStrikeoutPos(Float n)
        {
            _strikeoutPos = n;
        }

        /** @brief Returns the strikeout line thickness.
        */
        Float strikeoutThickness() const
        {
            return _strikeoutThickness;
        }

        /** @brief Sets the strikeout thickness.
        */
        void setStrikeoutThickness(Float n)
        {
            _strikeoutThickness = n;
        }

    private:
        Float _ascent;
        Float _descent;
        Float _capHeight;
        Float _xHeight;
        Float _leading;
        Float _underlinePos;
        Float _underlineThickness;
        Float _strikeoutPos;
        Float _strikeoutThickness;
};

} // namespace

} // namespace

#endif
