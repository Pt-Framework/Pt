/* Copyright (C) 2026 Marc Boris Duerner

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

#ifndef Pt_Forms_NanoVGDevice_h
#define Pt_Forms_NanoVGDevice_h

#include <Pt/Gfx/Font.h>

struct NVGcontext;

namespace Pt {

namespace Forms {

class NanoVGFontProvider;

// Owns the shared EGL/GLES2 context and the single nanovg context used by the
// nanovg renderer backend. The device is platform agnostic: the platform layer
// (e.g. the Wayland application) creates it from a native display handle and
// registers it as the process wide instance. Pixmaps and canvases render to
// offscreen framebuffers using this shared context, windows composite the
// result with the same context.
class NanoVGDevice
{
    public:
        NanoVGDevice();

        ~NanoVGDevice();

        // Creates the EGL context and the nanovg context for the given native
        // display. Returns false if EGL or GLES2 is not available. The native
        // display is an EGLNativeDisplayType (e.g. a wl_display*).
        bool create(void* nativeDisplay);

        void destroy();

        bool isValid() const
        { return _nvg != 0; }

        NVGcontext* context() const
        { return _nvg; }

        // EGL handles for window surface creation in the platform layer. The
        // values are EGLDisplay, EGLConfig and EGLContext stored as void* to
        // keep this header free of EGL headers.
        void* eglDisplay() const
        { return _display; }

        void* eglConfig() const
        { return _config; }

        void* eglContext() const
        { return _context; }

        // Makes the shared context current with the offscreen pbuffer surface.
        // Used before rendering to a pixmap framebuffer.
        void makeCurrentOffscreen();

        // Makes the shared context current with the given EGLSurface. Used by
        // windows before compositing into their swap chain.
        void makeCurrent(void* eglSurface);

        // Creates a nanovg image (texture) of the given size with
        // NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED flags. Returns the nanovg
        // image handle, or -1 on failure. The context must be current.
        int createImage(int w, int h);

        // Binds the shared render-target FBO with nvgImage as color attachment
        // and ensures the stencil RBO matches the target size. Calls
        // makeCurrentOffscreen() internally. After the call glViewport is set
        // to (0,0,w,h) and the FBO is ready to receive nvg draw calls.
        bool bindRenderTarget(int nvgImage, int w, int h);

        // Unbinds the shared render-target FBO (restores default framebuffer).
        void unbindRenderTarget();

        // Resolves a font request to a nanovg font handle, registering fonts on
        // first use. Returns -1 if no font is available.
        int fontFace(const Gfx::Font& font);

        // Returns the line-height/ascender ratio for a nanovg font handle, used
        // to convert a requested point size to a nanovg pixel size.
        float fontSizeScale(int handle);

        // Returns face->ascender / face->units_per_EM for a nanovg font handle.
        // Gives the true typographic ascender, bypassing the lineGap adjustment
        // that fontstash applies internally.
        float fontAscenderRatio(int handle);

        // Returns the cap height as a ratio of the ascender, from the OS/2
        // sCapHeight field. Returns 0.7 when not available.
        float fontCapHeightRatio(int handle);

        // Returns the x-height as a ratio of the ascender, from the OS/2
        // sxHeight field. Returns 0.54 when not available.
        float fontXHeightRatio(int handle);

        // Returns the shared render-target FBO handle for GPU blit operations.
        // Used by windows to efficiently composite the pixmap framebuffer.
        unsigned int renderFramebufferObject() const
        { return _renderFbo; }

        static NanoVGDevice* instance()
        { return _instance; }

    private:
        NanoVGDevice(const NanoVGDevice&);
        NanoVGDevice& operator=(const NanoVGDevice&);

    private:
        void*               _display;   // EGLDisplay
        void*               _config;    // EGLConfig
        void*               _context;   // EGLContext
        void*               _pbuffer;   // EGLSurface
        NVGcontext*         _nvg;
        NanoVGFontProvider* _fonts;

        unsigned int        _renderFbo;   // shared offscreen render-target FBO
        unsigned int        _stencilRbo;  // stencil RBO attached to _renderFbo
        int                 _rtW;         // current stencil RBO width
        int                 _rtH;         // current stencil RBO height

        static NanoVGDevice* _instance;
};

} // namespace

} // namespace

#endif
