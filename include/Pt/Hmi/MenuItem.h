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
   MA  02110-1301  USA
*/

#ifndef Pt_Hmi_MenuItem_H
#define Pt_Hmi_MenuItem_H

#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Font.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Hmi {

class Menu;

class PT_HMI_API MenuItem : public Control
{
    friend class Menu;

    typedef Control Base;

    public:
        MenuItem();
    
        virtual ~MenuItem();

        const String& text() const;

        void setText(const String& t);

        const Gfx::Image& icon() const;
        
        void setIcon(const Gfx::Image& img);

        double iconPadding() const;

        void setIconPadding(double left);

        Menu* subMenu() const;

        void setSubMenu(Menu& menu);

        // TODO: rename onSelected
        Signal<MenuItem&>& triggered();

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

        const std::string& fontStyle() const;

        void setFontStyle(const std::string& style);

        void setRenderer(MenuRenderer* renderer);

    protected:
        // TODO: rename onSelected
        virtual void onTriggered();
        
        virtual void onParentChanged(View* v);
        
        virtual void onShortcut(const KeyEvent& kev);

        virtual Gfx::SizeF onAutoSize(const SizePolicy& policy) const;

    protected:
        virtual void onInvalidate();
        
        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);
  
    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onEnterEvent( const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        Menu*             _menu;
        Signal<MenuItem&> _triggered;
        double            _iconWidth;
        Gfx::Image        _icon;
        Pt::String        _text;
        Menu*             _subMenu;

        FacetPtr<MenuRenderer> _renderer;
        bool                   _hasRenderer;

        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<std::string> _fontStyle;

        PixmapSurface     _picture;
        Gfx::Brush        _brush;
        Gfx::Pen          _pen;
        Gfx::Pen          _textPen;
        Gfx::Font         _font;
};

} // namespace

} // namespace

#endif
