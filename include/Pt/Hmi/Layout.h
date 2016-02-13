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

class LayoutItem
{
  public:
    virtual ~LayoutItem()
    {}

    virtual const Gfx::PointF& position() const = 0;

    virtual const Gfx::SizeF& size() const = 0;

    virtual const Spacing& padding() const = 0;

    virtual const Spacing& margin() const = 0;

    virtual const Docking& docking() const = 0;

    virtual void setGeometry(const Gfx::PointF& p, const Gfx::SizeF& s) = 0;
};


class Layouter;

class PT_HMI_API LayoutIterator
{
    public:
        LayoutIterator();

        explicit LayoutIterator(Layouter& layouter);

        LayoutIterator& operator++();
        
        bool operator!=(const LayoutIterator& other) const;

        bool operator==(const LayoutIterator& other) const;
        
        LayoutItem& operator*();

        LayoutItem* operator->();

    private:
        Layouter* _layouter;
        LayoutItem* _item;
        std::size_t _n;
};


class PT_HMI_API Layouter
{
    friend class LayoutIterator;

    public:
        virtual ~Layouter()
        {}

        virtual const Gfx::PointF& position() const = 0;

        virtual const Gfx::SizeF& size() const = 0;

        virtual const Spacing& padding() const = 0;

        LayoutIterator begin();

        LayoutIterator end();

    protected:
        virtual LayoutItem* onBegin() = 0;

        virtual LayoutItem* onAdvance() = 0;
};


class PT_HMI_API DockingLayout : public Widget
{
    public:
        DockingLayout();

        virtual ~DockingLayout();

    protected:
        virtual void onLayout(Layouter& layouter);
};


PT_HMI_API void StackLeft(Layouter& parent);

PT_HMI_API void StackRight(Layouter& parent);

PT_HMI_API void StackTop(Layouter& parent);

PT_HMI_API void StackBottom(Layouter& parent);

PT_HMI_API void Docked(Layouter& parent);

} // namespace

} // namespace

#endif // include guard
