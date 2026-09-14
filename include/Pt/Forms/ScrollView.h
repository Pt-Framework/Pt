/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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

#ifndef Pt_Forms_SCROLLVIEW_H
#define Pt_Forms_SCROLLVIEW_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/ScrollLayout.h>
#include <Pt/Forms/ScrollBar.h>

namespace Pt {

namespace Forms {

/** @brief Viewport over larger content.

    A %ScrollView presents one caller-owned content control that may
    be larger than the view. Use %setContent() to attach the control.
    Keep it alive until another call replaces it. Calling
    %setScrollBars() shows horizontal and vertical %ScrollBar objects
    when the content does not fit. %scrollX() and %scrollY() move the
    content. %setContentMode() chooses how the content is measured.

    The view has no family renderer. It hosts a %ScrollLayout and
    optional %ScrollBar objects. Offset of oversized content is a
    %ScrollLayout.

    @code
    Pt::Forms::Label terms;
    terms.setText("These terms apply to all deliveries. "
                  "Orders ship within five working days. "
                  "Damaged goods must be reported within 14 days.");

    Pt::Forms::ScrollView page;
    page.setContent(terms);
    page.setScrollBars(true);
    window.setContent(&page);
    @endcode

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ScrollView : public Control
{
    typedef Control Base;

    public:
        /** @brief Creates an empty scroll view.
        */
        ScrollView();

        /** @brief Destroys the scroll view. The content control is not destroyed.
        */
        virtual ~ScrollView();

        /** @brief Sets whether the view shows scroll bars.
        */
        void setScrollBars(bool hasScrollBars);

        /** @brief Sets the caller-owned content to @a control.

            Replaces the previous content control.
        */
        void setContent(Control& control);

        /** @brief Sets how the content is measured on each axis.
        */
        void setContentMode(SizePolicy::Mode horizontal, 
                            SizePolicy::Mode vertical);

        /** @brief Scrolls the content horizontally to @a xpos.
        */
        void scrollX(double xpos);

        /** @brief Scrolls the content vertically to @a ypos.
        */
        void scrollY(double ypos);

        /** @brief Returns the maximum horizontal scroll offset.
        */
        double maximumX() const;

        /** @brief Returns the maximum vertical scroll offset.
        */
        double maximumY() const;
        
    protected:
        /** @brief Scrolls the content horizontally from the horizontal bar.
        */
        void onScrollBarX(double pos);
        
        /** @brief Scrolls the content vertically from the vertical bar.
        */
        void onScrollBarY(double pos);

        /** @brief Updates the horizontal bar from the layout offset.
        */
        void onScrolledX(double n);
        
        /** @brief Updates the vertical bar from the layout offset.
        */
        void onScrolledY(double n);

    protected:
        /** @brief Measures the content and scroll bars.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the content and scroll bars in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);
    
    private:
        void updateScrollBar(ScrollBar& scroll, double maxRange);

    private:
        ScrollLayout _scrollLayout;
        bool         _hasScrollBars;
        Control*      _control;
        ScrollBar    _scrollBarX; 
        ScrollBar    _scrollBarY;
};

} // namespace

} // namespace

#endif