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
#include <Pt/Forms/Pixmap.h>

namespace Pt {

namespace Forms {

/** @brief Represents the common content and activation behavior of a menu item.

    A menu item displays text, an optional icon, an optional separator, and a
    shortcut when one is registered. A pointer or touch release over the item,
    or a matching shortcut, emits %triggered(). A highlighted item is the item
    currently under the pointer.

    The item derives its default drawing attributes from the application style.
    Its background, contour, text color, and font setters provide local
    overrides. %MenuItem, %MenuSubItem, %MenuMenuItem, and %MenuBarItem use
    this common behavior.

    @ingroup Pt-Forms-Menus
*/
class PT_FORMS_API MenuItemBase : public Control
{
    public:
        /** @brief Defines the inherited control type.
        */
        typedef Control Base;

    public:

        /** @brief Creates a menu item with empty content.
        */
        MenuItemBase();

        /** @brief Destroys the menu item.
        */
        virtual ~MenuItemBase();

        /** @brief Returns the item text.
        */
        const Pt::String& text() const;

        /** @brief Sets the item text.
        */
        void setText(const Pt::String& t);

        /** @brief Returns the item icon.
        */
        const Pt::Gfx::Image& icon() const;

        /** @brief Sets the item icon.
        */
        void setIcon(const Pt::Gfx::Image& img);

        /** @brief Returns the space reserved before the item icon.
        */
        double iconPadding() const;

        /** @brief Sets the space reserved before the item icon.

            The reserved space is at least the width of the current icon.
        */
        void setIconPadding(double left);

        /** @brief Sets whether a separator is drawn below this item.
        */
        void setSeperator(bool v)
        {
            _hasSeparator = v;
        }

        /** @brief Returns true if a separator is drawn below this item.
        */
        bool hasSeperator() const
        {
            return _hasSeparator;
        }

        /** @brief Returns true while the pointer is over this item.
        */
        bool isHighlighted() const
        {
            return _isHighlighted;
        }

        /** @brief Returns the signal emitted when this item is activated.
        */
        Pt::Signal<MenuItemBase&>& triggered();


    public:
        /** @brief Returns the background brush.

            Returns the local override when one was set; otherwise returns the
            application style background brush.
        */
        const Pt::Gfx::Brush& background() const;

        /** @brief Sets a local background brush.
        */
        void setBackground(const Pt::Gfx::Brush& b);

        /** @brief Returns the contour pen.

            Returns the local override when one was set; otherwise returns the
            application style contour pen.
        */
        const Pt::Gfx::Pen& contour() const;

        /** @brief Sets a local contour pen.
        */
        void setContour(const Pt::Gfx::Pen& p);

        /** @brief Returns the text color.

            Returns the local override when one was set; otherwise returns the
            application style text color.
        */
        const Pt::Gfx::Color& textColor() const;

        /** @brief Sets a local text color.
        */
        void setTextColor(const Pt::Gfx::Color& color);

        /** @brief Returns the item font.

            Returns the local full or partial override when one was set;
            otherwise returns the application style font.
        */
        const Pt::Gfx::Font& font() const;

        /** @brief Sets a local font.
        */
        void setFont(const Pt::Gfx::Font& font);

        /** @brief Sets a local font size.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets a local font weight.
        */
        void setFontWeight(Pt::Gfx::Font::Weight weight);

        /** @brief Sets a local font slant.
        */
        void setFontSlant(Pt::Gfx::Font::Slant slant);

    protected:
        static Pt::String shortcutText(const Pt::Forms::Key& key);

        virtual void onTriggered();

        virtual void onShortcut(const Pt::Forms::Key& key);

        virtual void onInvalidate();

        virtual Pt::Gfx::SizeF onMeasure(const Pt::Forms::SizePolicy& policy);

        virtual void onPaint(PaintContext& context, const Pt::Gfx::RectF& updateRect);

        virtual bool onMouseEvent(const Pt::Forms::MouseEvent& ev);

        virtual bool onTouchEvent(const Pt::Forms::TouchEvent& ev);

        virtual bool onEnterEvent(const Pt::Forms::EnterEvent& ev);

        virtual bool onLeaveEvent(const Pt::Forms::LeaveEvent& ev);

    private:
        Pt::Gfx::Font getFont() const;

    private:
        enum OverrideFlags : unsigned
        {
            OverrideBackground = 0x01,
            OverrideContour    = 0x02,
            OverrideTextColor  = 0x04,
            OverrideFontAll    = 0x08,
            OverrideFontSize   = 0x10,
            OverrideFontWeight = 0x20,
            OverrideFontSlant  = 0x40,
            OverrideFontAny    = OverrideFontAll | OverrideFontSize
                               | OverrideFontWeight | OverrideFontSlant
        };

    protected:
        Pt::Signal<MenuItemBase&> _triggered;
        double            _iconWidth;
        Pt::Gfx::Image        _icon;
        Pt::String        _text;

        Pt::AutoPtr<Pt::Gfx::Brush>       _background;
        Pt::AutoPtr<Pt::Gfx::Pen>         _contour;
        Pt::AutoPtr<Pt::Gfx::Color>       _textColor;
        Pt::Gfx::Font             _customFont;
        unsigned                  _overrides;

        Pt::Forms::Pixmap            _picture;
        Pt::Gfx::Brush        _brush;
        Pt::Gfx::Pen          _pen;
        Pt::Gfx::Pen          _textPen;
        Pt::Gfx::Font         _font;
        bool                  _hasSeparator;
        bool                  _isHighlighted;
};

}}

#endif
