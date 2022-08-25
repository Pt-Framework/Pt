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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_HMI_TABLELAYOUT_H
#define PT_HMI_TABLELAYOUT_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Layout.h>
#include <vector>

namespace Pt {

namespace Hmi {

class PT_HMI_API TableLayout : public Layout
{
    typedef Layout Base;

    public:
        // TODO: could be the same as Pt::Hmi::SizePolicy::Mode
        enum SizeMode
        {
            Fill, // -> Any
            Preferred,
            Fixed
        };

    public:
        TableLayout();
        
        virtual ~TableLayout();

        void addItem(Widget& w, std::size_t row, std::size_t column);

        void removeItem(Widget& w);

        void setColumn(std::size_t col, SizeMode mode, double size = 0);

        void setRow(std::size_t row, SizeMode mode, double size = 0);

    protected:
        virtual void onRemoveWidget(Widget& w);

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

    private:
        class SizeInfo
        {
            public:
                SizeInfo()
                : _mode(Preferred)
                , _size(0)
                { }

                SizeInfo(SizeMode m, double size)
                : _mode(m)
                , _size(size)
                { }

                SizeMode mode() const
                { return _mode; }

                double size() const
                { return _size; }

                void setSize(double s)
                { _size = s; }
            
            private:
                SizeMode _mode;
                double   _size;
        };

        std::vector<SizeInfo> _columnSizes;
        std::vector<SizeInfo> _rowSizes;

        typedef std::vector<Widget*> Row;
        std::vector<Row> _rows;
};

} // namespace

} // namespace

#endif
