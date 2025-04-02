 /* 
  Copyright (C) 2015 Marc Boris Duerner 
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
  MA  02110-1301  USA
*/

#ifndef Pt_Forms_ScreenImpl_H
#define Pt_Forms_ScreenImpl_H

#include "FrameBuffer.h"

#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Forms {

class ApplicationImpl;
class FrameBuffer;
class Cursor;

class ScreenImpl : public Pt::Connectable
{
    public:
        ScreenImpl(ApplicationImpl& app);

        virtual ~ScreenImpl();

        void registerWindow(Window& w);

        void unregisterWindow(Window& w);

        const FrameBuffer& frameBuffer() const
        {
            return _frameBuffer;
        }

        double width() const
        {
            return _frameBuffer.width();
        }

        double height() const
        {
            return _frameBuffer.height();
        }

        virtual void activate()
        {
        }

        const Gfx::Image& image() const;

        Gfx::Image& image();

        double toUnit(int value)
        {
            return value;
        }

            Gfx::PointF toUnit(const Gfx::Point& value)
        {
          return Gfx::PointF( value.x(), value.y() );
        }

        Gfx::SizeF toUnit(const Gfx::Size& value)
        {
            return Gfx::SizeF( value.width(), value.height() );
        }

        int fromUnit(double value)
        {
            return (int) value;
        }

        Gfx::Point fromUnit(const Gfx::PointF& value)
        {
            return Gfx::Point( (int)value.x(), (int)value.y() );
        }

        Gfx::Size fromUnit(const Gfx::SizeF& value)
        {
             return Gfx::Size( (int)value.width(), (int)value.height() );
        }

        Gfx::Rect fromUnit(const Gfx::RectF& value)
        {
            return Gfx::Rect(Gfx::Point( (int) value.x(), (int)value.y()) ,Gfx::Size( (int)value.width(), (int)value.height() ) );
        }

        double unitSizeInch() const
        {
            return 1.0/96;
        }

        double unitSizeMm() const
        {
            return 25.4 * unitSizeInch();
        }

        void setResolution(double dpi)
        {
            _dpi = dpi;
        }

        double resolutionDPI() const
        {
            return _dpi;
        }

        void setCursor(const Forms::Cursor* cursor );

        void update(const Gfx::RectF& updateRect);

        WindowManager& windowManager()
        {
            return _windowManager;
        }

    protected:
        virtual void onActivate();

        virtual void onMouseEvent( const Pt::Forms::MouseEvent& mouseEvent );

        virtual void onKeyEvent(const Pt::Forms::KeyEvent& ev);

    private:
        enum BlitOp
        {
            CopyOp,
            AndOp,
            XorOp
        };

        void grabImage( const Pt::uint8_t* buffer, const Gfx::Point& pos,Gfx::Image& image);
 
        void bitBlit( const Pt::uint8_t* , size_t width, size_t height, const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op );

        void bitBlit(const Gfx::Image& image, Pt::uint8_t* buffer);

        void drawCursor( Pt::uint8_t* buffer );

        void updateScreen();

    private:
        FrameBuffer&  _frameBuffer;
        Gfx::Image    _cursorBackground;
        Gfx::Point    _cursorPos;
        double        _dpi;
        bool          _drawCursor;
        WindowManager _windowManager;
        PixmapSurface _surface;
        Cursor        _cursor;
};

} // namespace

} // namespace

#endif
