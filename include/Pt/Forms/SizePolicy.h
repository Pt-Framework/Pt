/* Copyright (C) 2017 Marc Boris Duerner 
  
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_FORMS_SIZEPOLICY_H
#define PT_FORMS_SIZEPOLICY_H

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Forms {

/** @brief Constraint used when measuring a control.

    A %SizePolicy has a horizontal mode, a vertical mode, and an optional
    size hint. %Control::measure() combines the parent policy with the
    control's own policy.

    @ingroup Pt-Forms-Updating
*/
class SizePolicy
{
    public:
        /** @brief Constraint applied to one axis.
        */
        enum Mode
        {
            /** @brief The axis is not constrained by this policy.
            */
            Any = 0,

            /** @brief The axis uses the preferred size.
            */
            Preferred = 1,

            /** @brief The axis is capped by the policy size.
            */
            Maximum = 2,

            /** @brief The axis uses the policy size.
            */
            Fixed = 3,
        };

    public:
        /** @brief Creates a policy with unconstrained axes.
        */
        SizePolicy()
        : _horizontalMode(Any)
        , _verticalMode(Any)
        { }

        /** @brief Creates a policy with @a horizontal and @a vertical modes.
        */
        SizePolicy(Mode horizontal, Mode vertical)
        : _horizontalMode(horizontal)
        , _verticalMode(vertical)
        { }

        /** @brief Returns the horizontal mode.
        */
        Mode horizontal() const
        {
            return _horizontalMode;
        }

        /** @brief Sets the horizontal mode to @a m.
        */
        void setHorizontal(Mode m)
        {
            _horizontalMode = m;
        }

        /** @brief Returns the vertical mode.
        */
        Mode vertical() const
        {
            return _verticalMode;
        }

        /** @brief Sets the vertical mode to @a m.
        */
        void setVertical(Mode m)
        {
            _verticalMode = m;
        }

        /** @brief Sets both modes to @a horizontal and @a vertical.
        */
        void setMode(Mode horizontal, Mode vertical)
        {
            _horizontalMode = horizontal;
            _verticalMode = vertical;
        }

        /** @brief Returns the size hint.
        */
        const Gfx::SizeF& size() const
        {
            return _sizeHint;
        }

        /** @brief Sets the size hint to @a hint.
        */
        void setSize(const Gfx::SizeF& hint)
        {
            _sizeHint = hint;
        }

        /** @brief Sets the size hint to @a w by @a h.
        */
        void setSize(double w, double h)
        {
            _sizeHint.set(w, h);
        }

        /** @brief Returns the width hint.
        */
        double width() const
        {
            return _sizeHint.width();
        }

        /** @brief Sets the width hint to @a w.
        */
        void setWidth(double w)
        {
            _sizeHint.setWidth(w);
        }

        /** @brief Returns the height hint.
        */
        double height() const
        {
            return _sizeHint.height();
        }

        /** @brief Sets the height hint to @a h.
        */
        void setHeight(double h)
        {
            _sizeHint.setHeight(h);
        }

        /** @brief Returns true when both policies compare equal.
        */
        bool operator== (const SizePolicy& s) const
        {
            return _horizontalMode == s._horizontalMode &&
                   _verticalMode == s._verticalMode &&
                   _sizeHint.isEqual(s._sizeHint);
        }

        /** @brief Returns true when the policies differ.
        */
        bool operator!= (const SizePolicy& s) const
        {
            return _horizontalMode != s._horizontalMode ||
                   _verticalMode != s._verticalMode ||
                   ! _sizeHint.isEqual(s._sizeHint);
        }

    private:
        Mode       _horizontalMode;
        Mode       _verticalMode;
        Gfx::SizeF _sizeHint;
};

} // namespace

} // namespace

#endif // include guard
