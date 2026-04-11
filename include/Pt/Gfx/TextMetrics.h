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

#ifndef PT_GFX_TEXTMETRICS_H
#define PT_GFX_TEXTMETRICS_H

#include <Pt/Gfx/Api.h>

namespace Pt {

namespace Gfx {

/** @brief Metrics for a line of text.
*/
class PT_GFX_API TextMetrics
{
    public:
        TextMetrics();

        /** @brief Returns the advance width of the text.
        */
        Float advance() const
        {
            return _advance;
        }

        /** @brief Sets the advance width of the text.
        */
        void setAdvance(Float n)
        {
            _advance = n;
        }

        /** @brief Returns the horizontal bearing from origin to bounding box.
        */
        Float bearingX() const
        {
            return _bearingX;
        }

        /** @brief Sets the horizontal bearing.
        */
        void setBearingX(Float n)
        {
            _bearingX = n;
        }

        /** @brief Returns the vertical bearing from baseline to top of bounding box.
        */
        Float bearingY() const
        {
            return _bearingY;
        }

        /** @brief Sets the vertical bearing.
        */
        void setBearingY(Float n)
        {
            _bearingY = n;
        }

        /** @brief Returns the width of the bounding box.
        */
        Float boundingWidth() const
        {
            return _boundingWidth;
        }

        /** @brief Sets the bounding box width.
        */
        void setBoundingWidth(Float n)
        {
            _boundingWidth = n;
        }

        /** @brief Returns the height of the bounding box.
        */
        Float boundingHeight() const
        {
            return _boundingHeight;
        }

        /** @brief Sets the bounding box height.
        */
        void setBoundingHeight(Float n)
        {
            _boundingHeight = n;
        }

    private:
        Float _advance;
        Float _bearingX;
        Float _bearingY;
        Float _boundingWidth;
        Float _boundingHeight;
};

} // namespace

} // namespace

#endif // include guard
