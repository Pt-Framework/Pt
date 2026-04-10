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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/
#ifndef PT_FORMS_MENUITEMBASE_H
#define PT_FORMS_MENUITEMBASE_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/PixmapSurface.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API MenuBaseItem : public Control
{
    public:
        typedef Control Base;

    public:

        MenuBaseItem();

        virtual ~MenuBaseItem();

        const Pt::String& text() const;

        void setText(const Pt::String& t);

        const Pt::Gfx::Image& icon() const;

        void setIcon(const Pt::Gfx::Image& img);

        double iconPadding() const;

        void setIconPadding(double left);

        void setSeperator(bool v)
        {
            _hasSeparator = v;
        }

        bool hasSeperator() const
        {
            return _hasSeparator;
        }

        bool isHighlighted() const
        {
            return _isHighlighted;
        }

        Pt::Signal<MenuBaseItem&>& triggered();


    public:
        const Pt::Gfx::Brush& background() const;

        void setBackground(const Pt::Gfx::Brush& b);

        const Pt::Gfx::Pen& contour() const;

        void setContour(const Pt::Gfx::Pen& p);

        const Pt::Gfx::ColorF& textColor() const;

        void setTextColor(const Pt::Gfx::ColorF& color);

        const Pt::Gfx::Font& font() const;

        void setFont(const Pt::Gfx::Font& font);

    protected:
        static Pt::String shortcutText(const Pt::Forms::Key& key);

        virtual void onTriggered();

        virtual void onShortcut(const Pt::Forms::Key& key);

        virtual void onInvalidate();

        virtual Pt::Gfx::SizeF onMeasure(const Pt::Forms::SizePolicy& policy);

        virtual void onPaint(PaintSurface& surface, const Pt::Gfx::RectF& updateRect);

        virtual bool onMouseEvent(const Pt::Forms::MouseEvent& ev);

        virtual bool onTouchEvent(const Pt::Forms::TouchEvent& ev);

        virtual bool onEnterEvent(const Pt::Forms::EnterEvent& ev);

        virtual bool onLeaveEvent(const Pt::Forms::LeaveEvent& ev);

    protected:
        Pt::Signal<MenuBaseItem&> _triggered;
        double            _iconWidth;
        Pt::Gfx::Image        _icon;
        Pt::String        _text;

        Pt::AutoPtr<Pt::Gfx::Brush>       _background;
        Pt::AutoPtr<Pt::Gfx::Pen>         _contour;
        Pt::AutoPtr<Pt::Gfx::ColorF>       _textColor;
        Pt::AutoPtr<Pt::Gfx::Font> _fontValue;

        Pt::Forms::PixmapSurface     _picture;
        Pt::Gfx::Brush        _brush;
        Pt::Gfx::Pen          _pen;
        Pt::Gfx::Pen          _textPen;
        Pt::Gfx::Font         _font;
        bool                  _hasSeparator;
        bool                  _isHighlighted;
};

}}

#endif
