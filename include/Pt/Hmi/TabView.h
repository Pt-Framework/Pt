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

#ifndef Pt_Hmi_TabView_H
#define Pt_Hmi_TabView_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/StackLayout.h>
#include <Pt/Hmi/DockingLayout.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Hmi {

/** @brief Item for tab bars.
*/
class TabItem
{
    public:
        TabItem()
        : _isPressed(false)
        {}

        ~TabItem()
        {}

        const String& text() const
        { return _text; }

        void setText(const String& s)
        { _text = s; }

        const Gfx::RectF& geometry() const
        { return _geometry; }

        void setGeometry(const Gfx::RectF& r)
        { _geometry = r; }

        bool isPressed() const
        { return _isPressed; }

        void setPressed(bool b)
        { _isPressed = b; }

    private:
        String     _text;
        Gfx::RectF _geometry;
        bool       _isPressed;
};

/** @brief Tab bar for all tabbed widgets.
*/
class PT_HMI_API TabBar : public Control
{
    public:
        typedef Control Base;

    public:
        TabBar();

        virtual ~TabBar();

        bool empty() const;

        std::size_t size() const;

        void addTab(const Pt::String& title);

        void removeTab(std::size_t n);

        std::size_t current() const;

        void setCurrent(std::size_t n);

        void setText(std::size_t n, const Pt::String& title);

        Pt::Signal<std::size_t>& currentChanged()
        { return _currentChanged; }

    public:
        void setRenderer(TabViewRenderer* renderer);

    protected:
        virtual void onInvalidate();

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

    private:
        Pt::Signal<std::size_t> _currentChanged;
        std::vector<TabItem>    _tabs;
        std::size_t             _current;

        FacetPtr<TabViewRenderer> _renderer;
        bool                      _hasRenderer;

        Gfx::Brush  _backgroundBrush;
        Gfx::Brush  _foregroundBrush;
        Gfx::Pen    _contourPen;
        Gfx::Pen    _textPen;
        Gfx::Font   _font;
};

/** @brief Tabbed view for widgets.
*/
class PT_HMI_API TabView : public Control
{
    public:
        typedef Control Base;

    public:
        TabView();

        virtual ~TabView();

        bool empty() const;

        std::size_t size() const;

        void addTab(Widget& w, const Pt::String& title);

        void removeTab(std::size_t n);

        std::size_t current() const;

        void setCurrent(std::size_t n);

        void setText(std::size_t n, const Pt::String& title);

    public:
        void setRenderer(TabViewRenderer* renderer);

    protected:
        virtual void onInvalidate();

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

    private:
        DockingLayout             _layout;
        TabBar                    _tabBar;
        StackLayout               _stack;

        FacetPtr<TabViewRenderer> _renderer;
        bool                      _hasRenderer;

        Gfx::Brush                _backgroundBrush;
        Gfx::Brush                _foregroundBrush;
        Gfx::Pen                  _contourPen;
};

} // namespace

} // namespace

#endif
