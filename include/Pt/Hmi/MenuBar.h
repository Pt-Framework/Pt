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

#ifndef Pt_Hmi_MenuBar_H
#define Pt_Hmi_MenuBar_H

#include <Pt/Hmi/MenuShell.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/SmartPtr.h>
#include <vector>

namespace Pt {

namespace Hmi {

class Menu;
class MenuBar;

class MenuBarItem : public Control
{
    typedef Control Base;

    public:
        MenuBarItem(MenuBar& mb, Menu& menu, const Pt::String& text);

        ~MenuBarItem();

        Menu& menu()
        { return _menu; }

        const String& text() const;

        void setText(const String& t);

        void toggle();

        void open();

        void close();
    
    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        Gfx::Font::Style fontStyle() const;

        void setFontStyle(Gfx::Font::Style style);

        void setRenderer(MenuBarRenderer* renderer);

    protected:
        virtual Gfx::SizeF onAutoSize(const SizePolicy& policy) const;

        virtual void onInvalidate();

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual void onEnterEvent( const EnterEvent& ev);

        virtual void onLeaveEvent(const LeaveEvent& ev);

    private:
        MenuBar&   _menuBar;
        Menu&      _menu;
        Pt::String _text;

        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<Gfx::Font::Style> _fontStyle;

        FacetPtr<MenuBarRenderer> _renderer;
        bool                      _hasRenderer;

        Gfx::Brush               _brush;
        Gfx::Pen                 _pen;
        Gfx::Pen                 _textPen;
        Gfx::Font                _font;
};


class PT_HMI_API MenuBar : public MenuShell
                         , public Control
{
    typedef Control Base;

    public:
        MenuBar();
    
        virtual ~MenuBar();

        Menu* selectedMenu();

        MenuBarItem* findItem(const Gfx::PointF& pos);

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        void setRenderer(MenuBarRenderer* renderer);

    protected:
        virtual void onAddMenu(Menu& menu, const Pt::String& text);

        virtual void onRemoveMenu(Menu& menu);

        virtual void onOpenMenu(Menu& menu);

        virtual void onCloseMenu(Menu& menu);

        virtual void onCancel();

        virtual void onEnter();

        virtual MenuShell* onFindMenu(const Gfx::PointF& screenPos);

    protected:
        virtual void onInvalidate();

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        void onMenuTriggered(MenuBarItem& m);

    private:
        FlowLayout                _layout;
        std::vector<MenuBarItem*> _menus;
        Menu*                     _currentMenu;
        MenuBarItem*              _currentMenuItem;

        FacetPtr<MenuBarRenderer> _renderer;
        bool                      _hasRenderer;

        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Pen>          _contour;

        Gfx::Brush               _brush;
        Gfx::Pen                 _pen;
};

} // namespace

} // namespace

#endif
