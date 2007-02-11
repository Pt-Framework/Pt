/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

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

            virtual Math::Size minimumSize();

            virtual Math::Size preferredSize();

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
