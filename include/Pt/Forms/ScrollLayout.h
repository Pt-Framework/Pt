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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
	02110-1301  USA
*/

#ifndef Pt_Forms_ScrollLayout_H
#define Pt_Forms_ScrollLayout_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Layout.h>
#include <Pt/Forms/ScrollBar.h>

namespace Pt {

namespace Forms {

/** @brief Offsets children when content exceeds the viewport.

    Children are measured with %setContentMode() and placed at their
    preferred size. %scrollX() and %scrollY() move the content within the
    layout bounds. Offsets are clamped to the scrollable range.
    %enableScrolling() selects which axes accept scroll input. This layout
    does not draw scroll bars.

    @code
    +------------------+
    | visible window   |
    |            [====]|  content extends
    +------------------+
    @endcode

    @code
    Pt::Forms::ScrollLayout scroll;
    scroll.enableScrolling(true, true);
    scroll.addItem(content);
    window.setContent(&scroll);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class PT_FORMS_API ScrollLayout : public Layout
{
    typedef Layout Base;

    public:
        /** @brief Creates an empty scroll layout.
        */
        ScrollLayout();

        /** @brief Destroys the layout. Attached controls are not destroyed.
        */
        virtual ~ScrollLayout();

        /** @brief Enables horizontal scrolling when @a scrollX is true and
            vertical scrolling when @a scrollY is true.
        */
        void enableScrolling(bool scrollX, bool scrollY);

        /** @brief Returns the measured content extent on the X axis.
        */
        double maximumX() const;

        /** @brief Returns the measured content extent on the Y axis.
        */
        double maximumY() const;

        /** @brief Sets the horizontal scroll offset to @a xpos.

            The value is clamped to the scrollable range.
        */
        void scrollX(double xpos);

        /** @brief Sets the vertical scroll offset to @a ypos.

            The value is clamped to the scrollable range.
        */
        void scrollY(double ypos);

        /** @brief Returns the horizontal scroll offset.
        */
        double scrollPosX() const;

        /** @brief Returns the vertical scroll offset.
        */
        double scrollPosY() const;

        /** @brief Returns the signal emitted when the horizontal offset changes.
        */
        Pt::Signal<double>& scrolledX();

        /** @brief Returns the signal emitted when the vertical offset changes.
        */
        Pt::Signal<double>& scrolledY();

        /** @brief Attaches @a control as scrollable content.

            The layout does not take ownership.
        */
        void addItem(Control& control);

        /** @brief Detaches @a control without destroying it.
        */
        void removeItem(Control& control);

        /** @brief Sets the size-policy modes used to measure children.
        */
        void setContentMode(SizePolicy::Mode hmode, SizePolicy::Mode vmode);

    protected:
        /** @brief Measures children and records the content extent.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places children at their preferred size, offset by scroll.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    protected:
        /** @brief Forwards the mouse event to the base control.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Forwards the touch event to the base control.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Adjusts the scroll offset from @a ev and returns true.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);
       
    private:
        Pt::Signal<double> _scrolledX;
        Pt::Signal<double> _scrolledY;
        SizePolicy::Mode _hmode;
        SizePolicy::Mode _vmode;
        Gfx::PointF _scrollPos;
        double _scrollByX;
        double _scrollByY;
        bool _enableX;
        bool _enableY;
        double _maxX;
        double _maxY;
};

} // namespace

} // namespace

#endif // include guard
