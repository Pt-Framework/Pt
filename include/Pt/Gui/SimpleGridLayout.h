/*
 * Copyright (C) 2006 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_GUI_SIMPLEGRIDLAYOUT_H
#define PT_GUI_SIMPLEGRIDLAYOUT_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/LayoutManager.h>
#include <Pt/Gui/LayoutData.h>

#include <map>


namespace Pt {

namespace Gui {

    /**
     * !!Attention
     * LayoutManager are only inofficially supported by now. Use them at your own risk.
     * The documentation is not completed yet.
     */
    class PT_GUI_API SimpleGridLayoutData : public LayoutData
    {
        public:
            SimpleGridLayoutData(size_t x, size_t y, const Margin& margin = Margin(0, 0, 0, 0));

            //virtual SimpleGridLayoutData* clone() const;

            void setX(size_t x);

            void setY(size_t y);

            size_t x() const;

            size_t y() const;

        protected:
            size_t _x;
            size_t _y;
    };

    /**
     * !!Attention
     * LayoutManager are only inofficially supported by now. Use them at your own risk.
     * The documentation is not completed yet.
     */
    class PT_GUI_API SimpleGridLayout : public Layout
    {
        public:
            void setLayoutData(Widget& widget, const SimpleGridLayoutData& layoutData);

            void remove(Widget& widget);

            virtual void update();

            virtual Gfx::Size minimumSize();

            virtual Gfx::Size preferredSize();

            static SimpleGridLayout& create(
                Widget& widget,
                size_t columnCount,
                size_t rowCount,
                ssize_t horizontalGap = 0,
                ssize_t verticalGap = 0
            );

            size_t columnCount() const
            { return _columnCount; }

            size_t rowCount() const
            { return _rowCount; }

            ssize_t horizontalGap() const
            { return _horizontalGap; }

            ssize_t verticalGap() const
            { return _verticalGap; }

        private:
            SimpleGridLayout(Widget& widget, size_t columnCount, size_t rowCount, ssize_t horizontalGap, ssize_t verticalGap);

        private:
            size_t  _columnCount;
            size_t  _rowCount;
            ssize_t _horizontalGap;
            ssize_t _verticalGap;

            std::map<Widget*, SimpleGridLayoutData> _widget2LayoutData;
    };

} // namespace Gui

} // namespace Pt

#endif
