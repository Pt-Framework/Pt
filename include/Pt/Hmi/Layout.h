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

#ifndef Pt_Hmi_Layout_H
#define Pt_Hmi_Layout_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Spacing.h>
#include <Pt/Hmi/Docking.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Hmi {

class Layouter
{
    public:
        virtual ~Layouter()
        {}

        const Gfx::SizeF& size() const
        {
            return _size;
        }

        void setSize( const Gfx::SizeF& s )
        {
            _size = s;
        }

        const Spacing& padding() const
        {
            return _padding;
        }

        void setPadding(const Spacing& p)
        {
            _padding = p;
        }

        void update(LayoutItem::Iterator begin, LayoutItem::Iterator end)
        { onUpdate(begin, end); }

    protected:
        virtual void onUpdate(LayoutItem::Iterator begin, LayoutItem::Iterator end) = 0;

    private:
        Gfx::SizeF _size;
        Spacing    _padding;
};


class DockingLayouter : public Layouter
{
    public:
        virtual ~DockingLayouter()
        {}

    protected:
        virtual void onUpdate(LayoutItem::Iterator begin, LayoutItem::Iterator end);
};


class PT_HMI_API DockingLayout : public Widget
{
    public:
        DockingLayout();

        virtual ~DockingLayout();

    protected:
        virtual void onLayout(LayoutItem::Iterator begin, LayoutItem::Iterator end);

    private:
        DockingLayouter _layouter;
};


PT_HMI_API void StackLeft(LayoutItem& parent);

PT_HMI_API void StackRight(LayoutItem& parent);

PT_HMI_API void StackTop(LayoutItem& parent);

PT_HMI_API void StackBottom(LayoutItem& parent);

PT_HMI_API void Docked(LayoutItem& parent);

} // namespace

} // namespace

#endif // include guard
