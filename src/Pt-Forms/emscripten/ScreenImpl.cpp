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

#include <algorithm>
#include <cmath>
#include <cstdint>

#if defined(__wasm_simd128__)
#include <wasm_simd128.h>
#endif

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

// blits an RGBA8888 buffer onto the 2d context of the given canvas element at (dx, dy);
// proxied to the main thread since only it has DOM access under -sPROXY_TO_PTHREAD=1.
// putImageData() calls are throttled to at most one per browser animation frame: several
// correct-but-intermediate blits can otherwise be posted in quick succession (e.g. during a
// fast MDI subwindow resize) and the browser may composite in between two of them, flashing
// a stale frame before the final one appears
void blitCanvasImage2(const char* selector, const unsigned char* pixels, int width, int height,
                     int dx, int dy)
{
    MAIN_THREAD_EM_ASM({
        var canvas = document.querySelector(UTF8ToString($0));
        var ctx = canvas.ptContext2d || (canvas.ptContext2d = canvas.getContext("2d", { alpha: false }));
        var w = $2;
        var h = $3;
        var dx = $4;
        var dy = $5;
        var cw = canvas.width;
        var ch = canvas.height;

        var full = canvas.ptFullData;
        if (!full || full.w !== cw || full.h !== ch)
        {
            var buffer = new ArrayBuffer(cw * ch * 4);
            var offscreen = document.createElement("canvas");
            offscreen.width = cw;
            offscreen.height = ch;
            var offCtx = offscreen.getContext("2d", { alpha: false });

            // Object literal avoided: unparenthesized top-level commas would split the EM_ASM macro argument
            full = {};
            full.fastData = new Uint8Array(buffer);
            full.imgData = new ImageData(new Uint8ClampedArray(buffer), cw, ch);
            full.offscreen = offscreen;
            full.offCtx = offCtx;
            full.w = cw;
            full.h = ch;

            canvas.ptFullData = full;
            canvas.ptDirtyRects = [];
        }

        var x0 = Math.max(0, dx);
        var y0 = Math.max(0, dy);
        var x1 = Math.min(cw, dx + w);
        var y1 = Math.min(ch, dy + h);
        if(x1 <= x0 || y1 <= y0) return;

        var copyW = x1 - x0;
        var copyH = y1 - y0;
        var skipX = x0 - dx;
        var skipY = y0 - dy;

        // 1. ABSOLUT WICHTIG: Synchron kopieren, bevor C++ den Puffer (_canvasBuffer)
        // für das nächste Rechteck in Millisekunden überschreibt!
        var srcMem = HEAPU8;
        var dstMem = full.fastData;

        if (copyW === w && copyW === cw) {
            var srcPtr = $1 + (skipY * w) * 4;
            var dstPtr = y0 * cw * 4;
            dstMem.set(srcMem.subarray(srcPtr, srcPtr + copyH * w * 4), dstPtr);
        } else {
            var srcStride = w * 4;
            var dstStride = cw * 4;
            var rowBytes = copyW * 4;
            var srcPtr = $1 + ((skipY * w) + skipX) * 4;
            var dstPtr = (y0 * cw + x0) * 4;

            for(var y = 0; y < copyH; ++y) {
                dstMem.set(srcMem.subarray(srcPtr, srcPtr + rowBytes), dstPtr);
                srcPtr += srcStride;
                dstPtr += dstStride;
            }
        }

        if (!canvas.ptDirtyRects) {
            canvas.ptDirtyRects = [];
        }

        // 2. Erledigtes, perfekt aus C++ gerettetes Rectangle in die Todo-Liste pushen
        var newRect = {};
        newRect.x = x0;
        newRect.y = y0;
        newRect.w = copyW;
        newRect.h = copyH;
        canvas.ptDirtyRects.push(newRect);

        // 3. Atomares Flip-Scheduling (Double Buffering)
        if (!canvas.ptRafPending)
        {
            canvas.ptRafPending = true;

            // Ein Timeout debounced multiple Aufrufe. Wenn du ein MDI von Top/Left resizt,
            // haut Pt::Forms für den Hintergrund UND das Window in einem Lauf Paints raus.
            // Durch das Debouncen flippen wir erst, wenn Pt vollständig fertig ist.
            setTimeout(function() {
                requestAnimationFrame(function() {
                    canvas.ptRafPending = false;

                    var rects = canvas.ptDirtyRects;
                    canvas.ptDirtyRects = [];
                    var fullRef = canvas.ptFullData;

                    // Erst in den unsichtbaren Puffer...
                    for (var i = 0; i < rects.length; i++) {
                        var r = rects[i];
                        fullRef.offCtx.putImageData(fullRef.imgData, 0, 0, r.x, r.y, r.w, r.h);
                    }

                    // ... dann in einem Blitz ohne Tearing auf den Monitor
                    ctx.drawImage(fullRef.offscreen, 0, 0);
                });
            }, 0);
        }
    }, selector, pixels, width, height, dx, dy);
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


void ScreenImpl::updateCanvasBuffer(const Gfx::Image& image, std::size_t x0, std::size_t y0,
                                   std::size_t width, std::size_t height)
{
    const std::size_t srcStride = static_cast<std::size_t>(image.stride());
    const std::size_t dstStride = width * 4;

    _canvasBuffer.resize(dstStride * height);

    const Pt::uint8_t* src = image.data();
    Pt::uint8_t* dst = _canvasBuffer.data();

#if defined(__wasm_simd128__)
    // Maske, um das höchste Byte (Alpha im Little-Endian-Format) auf 0xFF zu setzen
    const v128_t alpha_mask = wasm_i32x4_splat(0xFF000000u);
#endif

    for(std::size_t y = 0; y < height; ++y)
    {
        const Pt::uint8_t* srcRow = src + (y0 + y) * srcStride + x0 * 4;
        Pt::uint8_t* dstRow = dst + y * dstStride;

        std::size_t x = 0;

#if defined(__wasm_simd128__)
        for(; x + 4 <= width; x += 4)
        {
            v128_t pixels = wasm_v128_load(srcRow + x * 4);

            // 1-Source Shuffle: nur R und B swappen, Alpha (Indizes 3, 7, 11, 15) ignorieren/behalten.
            // Übersetzt sich nativ in genau 1x pshufb (x86).
            v128_t swapped = wasm_i8x16_shuffle(pixels, pixels,
                2, 1, 0, 3,
                6, 5, 4, 7,
                10, 9, 8, 11,
                14, 13, 12, 15);

            // Alpha per OR fixieren. Übersetzt sich nativ in 1x por.
            v128_t out = wasm_v128_or(swapped, alpha_mask);

            wasm_v128_store(dstRow + x * 4, out);
        }
#endif

        for(; x < width; ++x)
        {
            Pt::uint32_t v;
            std::memcpy(&v, srcRow + x * 4, sizeof(v));

            Pt::uint32_t o = 0xFF000000u
                           | (v & 0x0000FF00u)
                           | ((v & 0x000000FFu) << 16)
                           | ((v & 0x00FF0000u) >> 16);

            std::memcpy(dstRow + x * 4, &o, sizeof(o));
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

    const Gfx::RectF updateRectP = scaling().toPhysical(updateRectF);

    const long imgWidth = static_cast<long>(image.width());
    const long imgHeight = static_cast<long>(image.height());

    // expand fractional edges outward by 1px so no seam/stale-pixel line remains
    const long x0 = std::max<long>(0, static_cast<long>( std::floor(updateRectP.x()) ) - 1);
    const long y0 = std::max<long>(0, static_cast<long>( std::floor(updateRectP.y()) ) - 1);
    const long x1 = std::min<long>(imgWidth,
        static_cast<long>( std::ceil(updateRectP.x() + updateRectP.width()) ) + 1);
    const long y1 = std::min<long>(imgHeight,
        static_cast<long>( std::ceil(updateRectP.y() + updateRectP.height()) ) + 1);

    if(x1 <= x0 || y1 <= y0)
        return;

    const std::size_t dirtyWidth = static_cast<std::size_t>(x1 - x0);
    const std::size_t dirtyHeight = static_cast<std::size_t>(y1 - y0);

    updateCanvasBuffer(image, static_cast<std::size_t>(x0), static_cast<std::size_t>(y0),
                       dirtyWidth, dirtyHeight);

    blitCanvasImage("canvas", _canvasBuffer.data(), int(dirtyWidth), int(dirtyHeight),
                    int(x0), int(y0));
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
