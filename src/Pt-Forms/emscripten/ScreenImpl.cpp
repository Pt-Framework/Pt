/* Copyright (C) 2015-2025 Marc Boris Duerner

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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Logger.h>

#include <emscripten.h>
#include <emscripten/html5.h>

PT_LOG_DEFINE("Pt.Forms.Screen")

namespace {

void resizeCanvasToWindow()
{
    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    emscripten_set_canvas_element_size("canvas", int(width), int(height));

    Pt::Forms::Screen& screen = Pt::Forms::Application::instance().screen();

    Pt::Gfx::SizeF size(width, height);
    size /= screen.scaleFactor();

    Pt::Forms::ResizeEvent rev(screen, size);
    Pt::Forms::Application::instance().processEvent(rev);

    Pt::Gfx::RectF updateRect(Pt::Gfx::PointF(0, 0), size);
    screen.repaint(updateRect);
}

// avoids emscripten_enter_soft_fullscreen's WebGL render-target resize path,
// which crashes when proxied to the main thread under -sPROXY_TO_PTHREAD=1
EM_BOOL onWindowResized(int /*eventType*/, const EmscriptenUiEvent* /*uiEvent*/, void* /*userData*/)
{
    resizeCanvasToWindow();
    return true;
}

// pins the canvas to the full browser viewport, independent of the host page's own CSS
void makeCanvasFullscreen(const char* selector)
{
    MAIN_THREAD_EM_ASM({
        var canvas = document.querySelector(UTF8ToString($0));
        document.body.style.margin = "0";
        document.body.style.overflow = "hidden";
        canvas.style.position = "fixed";
        canvas.style.top = "0";
        canvas.style.left = "0";
        canvas.style.width = "100vw";
        canvas.style.height = "100vh";
        canvas.style.display = "block";
    }, selector);
}

// blits an RGBA8888 buffer onto the 2d context of the given canvas element;
// proxied to the main thread since only it has DOM access under -sPROXY_TO_PTHREAD=1
void blitCanvasImage(const char* selector, const unsigned char* pixels, int width, int height)
{
    MAIN_THREAD_EM_ASM({
        var canvas = document.querySelector(UTF8ToString($0));
        var ctx = canvas.ptContext2d || (canvas.ptContext2d = canvas.getContext("2d"));
        // ImageData rejects a SharedArrayBuffer-backed view, so copy out of wasm memory
        var view = HEAPU8.subarray($1, $1 + $2 * $3 * 4);
        var data = new Uint8ClampedArray(view);
        ctx.putImageData(new ImageData(data, $2, $3), 0, 0);
    }, selector, pixels, width, height);
}

} // namespace

namespace Pt {

namespace Forms {

ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _parent(0)
, _genericBackend(0)
{
    makeCanvasFullscreen("canvas");

    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    emscripten_set_canvas_element_size("canvas", int(width), int(height));

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, onWindowResized);

    GraphicsBackend& graphics = Application::instance().graphicsBackend();
    _genericBackend = dynamic_cast<GenericGraphicsBackend*>(&graphics);
    if( ! _genericBackend )
        throw std::invalid_argument("invalid graphics");

    Gfx::SizeF size(width, height);
    _pixmap.reset(size);

    setSurface( &_pixmap, Gfx::PointF(0, 0) );
    setContent(&_workspace);
}


ScreenImpl::~ScreenImpl()
{
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, 0);

    Form::setSurface(0, Gfx::PointF(0, 0) );
}


void ScreenImpl::setParent(Screen* screen)
{
    _parent = screen;

    if(_parent)
    {
        Gfx::SizeF size = _pixmap.size();
        size /= scaleFactor();

        _parent->onResize(*this, size);
        _parent->onShow(*this, true);
    }

    onSetParent(_parent);
}


void ScreenImpl::addWindow(Window& w)
{
    _workspace.addWindow(w); 
}


void ScreenImpl::removeWindow(Window& w)
{ 
    _workspace.removeWindow(w); 
}


const std::vector<Window*>& ScreenImpl::windows() const
{
    return _workspace.windows();
}


WindowManager& ScreenImpl::windowManager()
{
    return _workspace.windowManager();
}


void ScreenImpl::setCapture(Widget* capture)
{
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

Widget* ScreenImpl::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    Widget* hit = Form::onHitTest(p);
    if(hit)
        return hit;

    return 0;
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


void ScreenImpl::onRequestResize(const Gfx::SizeF& s)
{
    if(_parent)
        _parent->onResize(*this, s);
}


///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ScreenImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);
}


void ScreenImpl::onEnable(bool e)
{
    Base::onEnable(e);
}


void ScreenImpl::onProcessMouseEvent(const MouseEvent& ev)
{
    Base::onProcessMouseEvent(ev);
}


bool ScreenImpl::onMouseEvent(const MouseEvent& ev)
{ 
    return Base::onMouseEvent(ev);
}


void ScreenImpl::onProcessTouchEvent(const TouchEvent& ev)
{ 
    Base::onProcessTouchEvent(ev);
}


bool ScreenImpl::onTouchEvent(const TouchEvent& ev)
{ 
    return Base::onTouchEvent(ev);
}


void ScreenImpl::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! isEnabled() )
        return;
  
    Base::onProcessScrollEvent(ev);
}


bool ScreenImpl::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void ScreenImpl::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! isEnabled() )
        return;
    
    Base::onProcessKeyEvent(ev);
}


bool ScreenImpl::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}


void ScreenImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);
}


void ScreenImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);

    Gfx::SizeF size = _pixmap.size();
    size /= scaleFactor();

    _pixmap.setScaleFactor( scaleFactor() );

    if(_parent)
        _parent->onResize(*this, size);
}


void ScreenImpl::onRescale(double scaling)
{
    Base::onRescale(scaling);
}


void ScreenImpl::onResizeEvent(const ResizeEvent& ev)
{
    if( size().isEqual(ev.size()) )
    {
        return;
    }

    Base::onResizeEvent(ev);

    Gfx::SizeF size = scaling().toPhysical( ev.size() );
    _pixmap.reset(size);
}


void ScreenImpl::updateCanvasBuffer(const Gfx::Image& image)
{
    const std::size_t width = static_cast<std::size_t>(image.width());
    const std::size_t height = static_cast<std::size_t>(image.height());
    const std::size_t srcStride = static_cast<std::size_t>(image.stride());
    const std::size_t dstStride = width * 4;

    _canvasBuffer.resize(dstStride * height);

    const Pt::uint8_t* src = image.data();
    Pt::uint8_t* dst = _canvasBuffer.data();

    // Argb32 stores B,G,R,A per pixel; canvas ImageData expects R,G,B,A
    for(std::size_t y = 0; y < height; ++y)
    {
        const Pt::uint8_t* srcRow = src + y * srcStride;
        Pt::uint8_t* dstRow = dst + y * dstStride;

        for(std::size_t x = 0; x < width; ++x)
        {
            dstRow[x * 4 + 0] = srcRow[x * 4 + 2];
            dstRow[x * 4 + 1] = srcRow[x * 4 + 1];
            dstRow[x * 4 + 2] = srcRow[x * 4 + 0];
            dstRow[x * 4 + 3] = 255;
        }
    }
}


void ScreenImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& updateRectF = ev.rect();

    if( updateRectF.isEmpty() )
        return;

    Base::onProcessPaintEvent(ev);

    const Gfx::Image& image = _genericBackend->image(_pixmap);
    updateCanvasBuffer(image);

    blitCanvasImage("canvas", _canvasBuffer.data(), int(image.width()), int(image.height()));
}


void ScreenImpl::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    PaintContext ctx(_pixmap);
    onPaint(ctx, rect);
}


void ScreenImpl::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    Gfx::Painter painter(context);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);
    painter.setBrush( Gfx::Color(40, 50, 80) );
    painter.fillRect(rect);
}

} // namespace

} // namespace
