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

#include "ScreenImpl.h"
#include "FrameBuffer.h"
#include "ApplicationImpl.h"
#include "MainWindowImpl.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>

#include <algorithm>
#include <cmath>
#include <cstring>

PT_LOG_DEFINE("Pt.Hmi.Screen")

namespace Pt {

namespace Hmi {

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _frameBuffer( app.frameBuffer() )
, _parent(0)
, _dpi(96.0)
, _cursorPos(0, 0)
, _drawCursor(false)
{
    Gfx::Size fs = _frameBuffer.size();
    Gfx::SizeF size( fs.width(), fs.height() );

    _surface.resize(size);
    _surface.pixmapImpl()->resize( _frameBuffer.size(), 
                                   _frameBuffer.strideSize() );

    Gfx::Painter painter(_surface);

    Gfx::RectF rect( Gfx::PointF(0, 0), _surface.size() );
    painter.setBrush( Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);

    _form.resize(size);
    _form.setContent(&_shell);
    _form.setParent(*this);

    updateScreen( Gfx::Rect(Gfx::Point(0, 0), _frameBuffer.size()) );
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if(_parent)
    {
        Gfx::Size fs = _frameBuffer.size();
        Gfx::SizeF size( fs.width(), fs.height() );

        _parent->onResize(*this, size);
        _parent->onShow(*this, true);
    }

    onSetParent(_parent);
}


void ScreenImpl::addWindow(Window& w)
{
    _shell.addWindow(w); 
}


void ScreenImpl::removeWindow(Window& w)
{ 
    _shell.removeWindow(w); 
}


const std::vector<Window*>& ScreenImpl::windows() const
{
    return _shell.windows();
}


//double ScreenImpl::scaleFactor() const
//{
//    return _surface.scaleFactor();
//}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* ScreenImpl::onHitTest(const Gfx::PointF& pos)
{
    return _form.hitTest(pos);
}


Gfx::PointF ScreenImpl::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->toParent(pos);
}


Gfx::PointF ScreenImpl::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->fromParent(pos);
}


void ScreenImpl::onProcessEvent(const Event& ev)
{
    Base::onProcessEvent(ev);
}


void ScreenImpl::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->repaint(rect);
}

///////////////////////////////////////////////////////////////////////
// Sheet
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onAttach(Form& form)
{
    Sheet::onAttach(form);
}

    
void ScreenImpl::onDetach(Form& form)
{
    Sheet::onDetach(form);
}


void ScreenImpl::onInit(Form& form)
{
    form.setSurface(&_surface, Gfx::PointF(0, 0) );
    form.setNextResponder(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(form, scaling);
    form.processEvent(ev);
    //Application::instance().loop().commitEvent(ev);
}


void ScreenImpl::onRelease(Form& form)
{
    form.setSurface( 0, Gfx::PointF(0, 0) );
    form.setNextResponder(0);
}


Gfx::PointF ScreenImpl::onFromForm(const Form& form, const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF ScreenImpl::onToForm(const Form& form,  const Gfx::PointF& pos) const
{
    return pos;
}


void ScreenImpl::onRepaintRequest(Form& s, const Gfx::RectF& rect)
{
    Gfx::PointF clientPos = onFromForm( s, rect.topLeft() );
    Gfx::RectF clientRect( clientPos, rect.size() );

    repaint(clientRect);
}


void ScreenImpl::onActivateRequest(Form& w, bool active)
{
}


void ScreenImpl::onShowRequest(Form& form, bool isShow)
{
    ShowEvent ev(form, isShow);
    Application::instance().commitEvent(ev);
}


void ScreenImpl::onMoveRequest(Form& form, const Gfx::PointF& pos)
{   
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = _surface.align(pos);

    //
    // send move event
    //
    MoveEvent mev(form, aligedPos);
    ////Application::instance().processEvent(mev);
    Application::instance().commitEvent(mev);

    //
    // request repaint
    //
    Gfx::RectF updateRect( form.position(), form.size() );
    Gfx::RectF movedRect( aligedPos, form.size() );
    updateRect.unify(movedRect);

    repaint(updateRect);
}


void ScreenImpl::onResizeRequest(Form& form, const Gfx::SizeF& size)
{
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = _surface.align(size);

    //
    // send resize event
    //
    ResizeEvent rev(form, alignedSize);
    ////Application::instance().processEvent(rev);
    Application::instance().commitEvent(rev);

    //
    // request repaint
    //
    Gfx::RectF updateRect( form.position(), form.size() );
    Gfx::RectF resizedRect( form.position(), alignedSize );
    updateRect.unify(resizedRect);

    repaint(updateRect);
}


void ScreenImpl::setCapture(Visual* capture)
{
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

//bool ScreenImpl::isEnabled() const
//{
//    return _enabledState;
//}


void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    //bool wasEnabled = isEnabled();

    //_enabledState = ev.enabled();

    //if( wasEnabled != isEnabled() )
    //{
    //    onEnable( ev.enabled() );
    //}

    //bool enable = ev.enabled();
    //if( ! isEnabled() )
    //  enable = false;

    EnableEvent formEvent(_form, ev.enabled());
    Application::instance().loop().commitEvent(formEvent);
}


void ScreenImpl::onEnable(bool e)
{
    Base::onEnable(e);
}


void ScreenImpl::onProcessMouseEvent(const MouseEvent& ev)
{
    _form.processEvent(ev);
}


bool ScreenImpl::onMouseEvent(const MouseEvent& ev)
{ 
    return Base::onMouseEvent(ev);
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{ 
    _form.processEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{ 
    return Base::onTouchEvent(ev);
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! isEnabled() )
        return;
  
    _form.processEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! isEnabled() )
        return;
    
    _form.processEvent(ev);
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}


void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    RescaleEvent formEvent(_form, scaling);
    _form.processEvent(formEvent);
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void ScreenImpl::onRescale(double scaling)
{
    Base::onRescale(scaling);

    _surface.setScaleFactor(scaling);
}


void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& screenRect = ev.rect();

    if( screenRect.isNull() )
        return;

    Base::onProcessPaintEvent(ev);

    //
    //
    // paint form
    //
    Gfx::RectF updateRect = bounds().intersect(screenRect);
 
    if( ! updateRect.isNull() )
    {
        PaintEvent pev( _form, updateRect );
        _form.processEvent(pev);
    }

    //std::clog << "screen update2: " << clock.stop().toUSecs() << " usecs." << std::endl;
    //std::clog << "update area2: " << updateRect.topLeft().x() << ',' << updateRect.topLeft().y()
    //          << ' ' << updateRect.width() << 'x' << updateRect.height() << std::endl;

    //
    // update the screen including the cursor
    //
    Pt::Gfx::RectF urect = _surface.toPhysical(screenRect);
    updateScreen( Gfx::round(urect) );
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    onPaint(_surface, rect);
}


void ScreenImpl::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    //
    // erase previous cursor area in back buffer
    //
    if( ! _cursorBackground.empty() )
    {
        bitBlit( _cursorBackground.data(),
                 _cursorBackground.width(),
                 _cursorBackground.height(),
                 _cursorPos,
                 image().data(), CopyOp );
    }

    //
    // repaint the update area
    //
    Gfx::Painter painter(_surface);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Pt::Gfx::Color(0, 0, 0) );
    painter.fillRect(rect);
}


const Gfx::Image& ScreenImpl::image() const
{
    return _surface.pixmapImpl()->image();
}


Gfx::Image& ScreenImpl::image()
{
    return _surface.pixmapImpl()->image();
}


void ScreenImpl::updateScreen(const Gfx::Rect& r)
{
    if(_drawCursor)
        drawCursor( image().data() );
    
    _frameBuffer.output( image().data(), r );
}


void ScreenImpl::drawCursor(const Pt::Gfx::PointF& pos)
{
    _drawCursor = true;
 
    //
    // erase previous cursor area on screen
    //
    if( ! _cursorBackground.empty() )
    {
        PT_LOG_DEBUG("erasing cursor image");

        bitBlit( _cursorBackground.data(), 
                 _cursorBackground.width(), _cursorBackground.height(),
                 _cursorPos, (Pt::uint8_t*)image().data(), CopyOp );

       // TODO: is this enough to clear the cursor area in the back buffer?
       _frameBuffer.output( image().data(), 
                            Gfx::Rect(_cursorPos, _cursorBackground.size()) );
    }

    //
    // calculate new cursor area
    //
    const Cursor& cursor = Application::instance().impl()->cursor();

    _cursorPos = Gfx::Point( pos.x() - cursor.xHotspot(),
                             pos.y() - cursor.yHotspot() );

    PT_LOG_DEBUG("cursor hotspot position: " << _cursorPos.x() << "," << _cursorPos.y());

    Gfx::Rect cursorArea = Gfx::Rect(_cursorPos,
                                     Gfx::Size(cursor.width(), cursor.height()));

    //
    // update the screen including the new cursor image
    //
    PT_LOG_DEBUG("update cursor area: " << cursor.width() << "x" << cursor.height());
    updateScreen(cursorArea);
}


void ScreenImpl::drawCursor(Pt::uint8_t* buffer)
{
    const Cursor& cursor = Application::instance().impl()->cursor();
    if( cursor.empty() )
    {
        PT_LOG_DEBUG("no cursor image");
        return;
    }
    
    if( _cursorBackground.width() != static_cast<int>( cursor.width() ) ||
        _cursorBackground.height() != static_cast<int>( cursor.height() ) )
    {
        Gfx::Size size( cursor.width(), cursor.height() );
        _cursorBackground.reset(_frameBuffer.format(), size);
    }

    // keep the of background of the cursor area
    PT_LOG_DEBUG("saving cursor background");
    grabImage( buffer, _cursorPos, _cursorBackground );

    // draw cursor to the buffer
    PT_LOG_DEBUG("drawing cursor image");

    bitBlit(&cursor.andRgb888()[0], cursor.width(), cursor.height(), 
            _cursorPos, buffer, AndOp);
    
    bitBlit(&cursor.xorRgb888()[0], cursor.width(), cursor.height(), 
            _cursorPos, buffer, XorOp);
}


void ScreenImpl::grabImage(const Pt::uint8_t* buffer, 
                           const Gfx::Point& pos, 
                           Gfx::Image& image)
{
    const size_t pixelSizeInByte = _frameBuffer.pixelSize();
    const Gfx::Size& imageSize = image.size();
    const Pt::ssize_t yMax = std::min<Pt::ssize_t>(pos.y() + imageSize.height(), _frameBuffer.height() );

    int fbWidth = static_cast<int>( _frameBuffer.width() );

    size_t widthInPixel = ( pos.x() + imageSize.width() ) < fbWidth ? imageSize.width()
                                                                    : fbWidth - pos.x();
    const size_t widthInByte = widthInPixel * pixelSizeInByte;

    for(Pt::ssize_t y = pos.y(); y < yMax; ++y)
    {
        size_t lineOffset = y * _frameBuffer.lineSize() +
                            pos.x() * pixelSizeInByte;

        Pt::uint8_t* pdata = image.data() + (y - pos.y()) * image.view().stride();
        std::memcpy( pdata, &buffer[lineOffset], widthInByte );
    }
}


void ScreenImpl::bitBlit( const Pt::uint8_t* plane, size_t w, size_t h,
                          const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op )
{
    static const size_t planePixelSize = 4;
    const size_t bufferPixelSize = _frameBuffer.pixelSize();
    const size_t bufferWidth  = std::min<size_t>( pos.x() + w, _frameBuffer.width() );
    const size_t bufferHeight = std::min<size_t>( pos.y() + h, _frameBuffer.height() );
    size_t yCursor = 0;
    size_t xCursor = 0;

    for( size_t yBuffer = pos.y(); yBuffer < bufferHeight; ++yBuffer, ++yCursor )
    {
        const size_t lineOffsetBuffer  = yBuffer * _frameBuffer.lineSize();
        const size_t lineOffsetCursor  = yCursor * (w * planePixelSize);

        xCursor = 0;

        for( size_t xBuffer = pos.x(); xBuffer < bufferWidth; ++xBuffer, ++xCursor  )
        {
            Pt::uint8_t* pointerBuffer = &((Pt::uint8_t*)buffer)[lineOffsetBuffer + (xBuffer * bufferPixelSize)];
            const Pt::uint8_t* pointerCursor = &plane[lineOffsetCursor + (xCursor * planePixelSize)];

            switch( _frameBuffer.depth() )
            {
                case 32:
                {
                    Pt::uint32_t* pixelBuffer = (Pt::uint32_t*) pointerBuffer;
                    const Pt::uint32_t* pixelCursor = (const Pt::uint32_t*) pointerCursor;

                    switch( op )
                    {
                        case AndOp:
                            *pixelBuffer &= *pixelCursor;
                        break;

                        case XorOp:
                            *pixelBuffer ^= *pixelCursor;
                        break;

                        case CopyOp://ToDo::optimize this with memcpy
                            *pixelBuffer = *pixelCursor;
                        break;
                    }
                    break;
                }

                case 16:
                    //TODO:
                    break;
            }
        }
    }
}

} // namespace

} // namespace
