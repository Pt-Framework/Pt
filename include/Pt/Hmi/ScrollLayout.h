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

#ifndef Pt_Hmi_ScrollLayout_H
#define Pt_Hmi_ScrollLayout_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Layout.h>
#include <Pt/Hmi/ScrollBar.h>

namespace Pt {

namespace Hmi {

// TODO: should this really be a Layout? onLayout is not overrriden
//       -> rename ScrollArea and derive from Widget
class PT_HMI_API ScrollLayout : public Layout
{
    public:
        ScrollLayout();

        virtual ~ScrollLayout();

        void enableScrolling(bool scrollX, bool scrollY);

        int maximumX() const;

        int maximumY() const;

        void scrollX(int xpos);

        void scrollY(int ypos);

        int scrollPosX() const;

        int scrollPosY() const;

        Pt::Signal<int>& scrolledX();

        Pt::Signal<int>& scrolledY();

        Pt::Signal<>& contentChanged();

    protected:
        virtual void onMouseEvent(const MouseEvent& ev);
        
        virtual void onTouchEvent(const TouchEvent& ev);

        virtual void onScrollEvent(const ScrollEvent& ev);
        
        virtual void onAddWidget(Widget& w);
        
        virtual void onRemoveWidget(Widget& w);

  private:
        void onContentResize(const ResizeEvent& ev);

        void onContentMove(const MoveEvent& ev);
        
        void updateRange();

    private:
        Pt::Signal<int> _scrolledX;
        Pt::Signal<int> _scrolledY;
        Pt::Signal<> _contentChanged;
        Gfx::PointF _lastScrollPos;
        Gfx::PointF _lastPos;
        bool _enableX;
        bool _enableY;
        int _maxX;
        int _maxY;
};

} // namespace

} // namespace

#endif // include guard
