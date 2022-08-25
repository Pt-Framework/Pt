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

#ifndef PT_HMI_SPINBOX_H
#define PT_HMI_SPINBOX_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/LineEditor.h>
#include <Pt/Hmi/Adjustment.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API SpinBoxButton : public Button
{
    public:
        typedef Button Base;

    public:
        enum Type
        {
          Up, 
          Down
        };

    public:
        SpinBoxButton(Type type);

        ~SpinBoxButton();

        Type type() const;

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        void setRenderer(SpinBoxRenderer* renderer);

    protected:
        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual void onInvalidate();

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    private:
        Type                      _type;
        
        FacetPtr<SpinBoxRenderer> _renderer;
        bool                      _hasRenderer;

        AutoPtr<Gfx::Brush>       _foreground;
        AutoPtr<Gfx::Pen>         _contour;

        Gfx::Brush                _brush;
        Gfx::Pen                  _pen;
};


class PT_HMI_API SpinBox : public Control
{
    public:
        typedef Control Base;

    public:
        SpinBox();

        ~SpinBox();

        bool isEditable() const;

        void setEditable(bool e);

        int minimum() const;

        int maximum() const;

        void setRange(int min, int max);

        int value() const;

        void setValue(int n);

        const Pt::String& text() const;

        bool isEmpty() const;

        Adjustment textAdjustment() const;

        void setTextAdjustment(Adjustment a);

        std::size_t cursorPosition() const;

        void setCursorPosition(std::size_t n);

        bool isAccepted() const;

        void setAccepted(bool a);

        Pt::Signal<int>& valueEdited();

        Pt::Signal<const Pt::String&>& returnPressed();

        Pt::Signal<const Pt::String&>& editingFinished();

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

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

        void setRenderer(SpinBoxRenderer* renderer);

    protected:
        virtual Pt::String toText(int n) const;

        virtual bool toValue(const Pt::String& str, int& n) const;

        virtual bool onInput(const Pt::String& str) const;

        virtual void onStep(int n);

    private:
        bool setInput(const Pt::String& str);

        void onUp();

        void onDown();

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onInvalidate();

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    protected:
        virtual bool onKeyEvent(const KeyEvent& ev);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual void onFocusEvent(const FocusEvent& ev);

    private:
        Pt::Signal<int>               _valueEdited; 
        Pt::Signal<const Pt::String&> _returnPressed;
        Pt::Signal<const Pt::String&> _editingFinished;

        LineEditor                    _editor;
        TextLine                      _line;
        bool                          _isEditable;
        bool                          _isAccepted;
        bool                          _isTextChanged;
        int                           _value;
        int                           _minimum;
        int                           _maximum;

        SpinBoxButton                 _downButton;
        SpinBoxButton                 _upButton;
        Gfx::RectF                    _textBox;
        Gfx::SizeF                    _buttonSize;
        double                        _spacing;

        FacetPtr<SpinBoxRenderer>     _renderer;
        bool                          _hasRenderer;

        AutoPtr<Gfx::Brush>           _background;
        AutoPtr<Gfx::Pen>             _contour;
        AutoPtr<Gfx::Color>           _textColor;
        AutoPtr<std::string>          _fontName;
        AutoPtr<std::size_t>          _fontSize;
        AutoPtr<std::string>     _fontStyle;

        Gfx::Brush                    _backgroundBrush;
        Gfx::Pen                      _pen;
        Gfx::Pen                      _textPen;
        Gfx::Font                     _font;
};

} // namespace

} // namespace

#endif
