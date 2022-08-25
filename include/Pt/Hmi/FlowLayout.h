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
	02110-1301 USA
*/

#ifndef Pt_Hmi_FlowLayout_H
#define Pt_Hmi_FlowLayout_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Layout.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API FlowLayout : public Layout
{
    typedef Layout Base;

    public:
        // Horizontal          use all space in row, same size for elements
        // HorizontalCenter    place elements accoring to size
        // Vertical            use all space in row, same size for elements
        // VerticalCenter      place elements accoring to size
        
        enum Direction
        {
            Left,
            Right,
            Top,
            Bottom
        };

    public:
        explicit FlowLayout(Direction d = Left);

        virtual ~FlowLayout();

        void setDirection(Direction d);

        void setCenter(bool b);

        void setReverse(bool b);

        void addItem(Widget& w);

        void removeItem(Widget& w);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

    private:
        Gfx::SizeF onMeasureHorizontal(const SizePolicy& policy);

        Gfx::SizeF onMeasureVertical(const SizePolicy& policy);

        void onLayoutLeft(const Gfx::RectF& rect, bool center);

        void onLayoutRight(const Gfx::RectF& rect, bool center);

        void onLayoutTop(const Gfx::RectF& rect, bool center);

        void onLayoutBottom(const Gfx::RectF& rect, bool center);

    private:
        Direction _direction;
        bool      _center;
        bool      _reverse;
};

} // namespace

} // namespace

#endif // include guard
