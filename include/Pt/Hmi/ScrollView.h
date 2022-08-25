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

#ifndef Pt_Hmi_SCROLLVIEW_H
#define Pt_Hmi_SCROLLVIEW_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/ScrollLayout.h>
#include <Pt/Hmi/ScrollBar.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API ScrollView : public Widget
{
    typedef Widget Base;

    public:
        ScrollView();

        virtual ~ScrollView();

        void setScrollBars(bool hasScrollBars);

        void setContent(Widget& w);

        void setContentMode(SizePolicy::Mode horizontal, 
                            SizePolicy::Mode vertical);

        void scrollX(double xpos);

        void scrollY(double ypos);

        double maximumX() const;

        double maximumY() const;
        
    protected:
        void onScrollBarX(double pos);
        
        void onScrollBarY(double pos);

        void onScrolledX(double n);
        
        void onScrolledY(double n);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);
    
    private:
        void updateScrollBar(ScrollBar& scroll, double maxRange);

    private:
        ScrollLayout _scrollLayout;
        bool         _hasScrollBars;
        Widget*      _widget;
        ScrollBar    _scrollBarX; 
        ScrollBar    _scrollBarY;
};

} // namespace

} // namespace

#endif