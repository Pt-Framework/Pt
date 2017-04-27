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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef Pt_Hmi_StackLayout_H
#define Pt_Hmi_StackLayout_H

#include <Pt/Hmi/Layout.h>
#include <Pt/Signal.h>
#include <vector>

namespace Pt {

namespace Hmi {

class PT_HMI_API StackLayout : public Layout
{
    public:
        typedef Layout Base;
        static const std::size_t NoIndex = static_cast<const std::size_t>(-1); 

    public:
        StackLayout();

        virtual ~StackLayout();

        void addItem(Widget& w);

        void removeItem(Widget& w);

        bool empty() const;

        std::size_t size() const;

        Widget* widgetAt(std::size_t n) const;

        std::size_t indexOf(Widget& w) const;

        std::size_t current() const;

        void setCurrent(std::size_t n);

        Pt::Signal<std::size_t>& widgetRemoved()
        { return _widgetRemoved; }

        Pt::Signal<std::size_t>& currentChanged()
        { return _currentChanged; }

    protected:
        virtual void onRemoveWidget(Widget& w);

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

    private:
        Pt::Signal<std::size_t> _widgetRemoved;
        Pt::Signal<std::size_t> _currentChanged;
        std::vector<Widget*>    _widgets;
        std::size_t             _current;
};

} // namespace

} // namespace

#endif
