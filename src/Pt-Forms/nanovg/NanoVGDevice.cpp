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

#include "NanoVGDevice.h"
#include "NanoVGFontProvider.h"

#include <Pt/System/Logger.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "nanovg.h"
#include "nanovg_gl.h"

PT_LOG_DEFINE("Pt.Forms.NanoVG");

namespace Pt {

namespace Forms {

NanoVGDevice* NanoVGDevice::_instance = 0;

NanoVGDevice::NanoVGDevice()
: _display(0)
, _config(0)
, _context(0)
, _pbuffer(0)
, _nvg(0)
, _fonts(0)
, _renderFbo(0)
, _stencilRbo(0)
, _rtW(0)
, _rtH(0)
, _quadProgram(0)
, _quadVbo(0)
, _quadPosAttr(-1)
, _quadTexUniform(-1)
{
    _instance = this;
}


NanoVGDevice::~NanoVGDevice()
{
    destroy();

    if(_instance == this)
        _instance = 0;
}


bool NanoVGDevice::create(void* nativeDisplay)
{
    if(_nvg)
        return true;

    EGLDisplay display = eglGetDisplay(static_cast<EGLNativeDisplayType>(nativeDisplay));
    if(display == EGL_NO_DISPLAY)
    {
        PT_LOG_ERROR("eglGetDisplay failed");
        return false;
    }

    EGLint major = 0;
    EGLint minor = 0;
    if( ! eglInitialize(display, &major, &minor))
    {
        PT_LOG_ERROR("eglInitialize failed");
        return false;
    }

    if( ! eglBindAPI(EGL_OPENGL_ES_API))
    {
        PT_LOG_ERROR("eglBindAPI failed: " << eglGetError());
        eglTerminate(display);
        return false;
    }

    // Try config with both window and pbuffer surfaces (works on imx8/Vivante).
    // Some EGL implementations (e.g. Mesa on WSL) only expose configs for one
    // surface type at a time, so fall back to a window-only config.
    const EGLint configWithPbuf[] = {
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_STENCIL_SIZE,    8,
        EGL_NONE
    };

    const EGLint configWindowOnly[] = {
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_STENCIL_SIZE,    8,
        EGL_NONE
    };

    EGLConfig config = 0;
    EGLint numConfigs = 0;
    bool hasPbufferBit = eglChooseConfig(display, configWithPbuf,
                                         &config, 1, &numConfigs)
                         && numConfigs >= 1;

    if( ! hasPbufferBit)
    {
        eglChooseConfig(display, configWindowOnly, &config, 1, &numConfigs);
        if(numConfigs < 1)
        {
            PT_LOG_ERROR("eglChooseConfig failed (error " << eglGetError() << ")");
            eglTerminate(display);
            return false;
        }
    }

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if(context == EGL_NO_CONTEXT)
    {
        PT_LOG_ERROR("eglCreateContext failed: " << eglGetError());
        eglTerminate(display);
        return false;
    }

    // Prefer a 1x1 pbuffer for the shared offscreen surface. If the chosen
    // config does not include EGL_PBUFFER_BIT (window-only fallback above),
    // skip pbuffer creation and rely on EGL_KHR_surfaceless_context instead;
    // passing EGL_NO_SURFACE to eglMakeCurrent is valid when that extension
    // is present (Mesa, Vivante, and virtually all ES2+ drivers support it).
    EGLSurface pbuffer = EGL_NO_SURFACE;
    if(hasPbufferBit)
    {
        const EGLint pbufferAttribs[] = {
            EGL_WIDTH,  1,
            EGL_HEIGHT, 1,
            EGL_NONE
        };
        pbuffer = eglCreatePbufferSurface(display, config, pbufferAttribs);
        if(pbuffer == EGL_NO_SURFACE)
        {
            PT_LOG_WARN("eglCreatePbufferSurface failed (" << eglGetError()
                        << "), using surfaceless context");
        }
    }

    if( ! eglMakeCurrent(display, pbuffer, pbuffer, context))
    {
        PT_LOG_ERROR("eglMakeCurrent failed: " << eglGetError());
        if(pbuffer != EGL_NO_SURFACE)
            eglDestroySurface(display, pbuffer);
        eglDestroyContext(display, context);
        eglTerminate(display);
        return false;
    }

    NVGcontext* nvg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if( ! nvg)
    {
        PT_LOG_ERROR("nvgCreateGLES2 failed");
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(display, pbuffer);
        eglDestroyContext(display, context);
        eglTerminate(display);
        return false;
    }

    _display = display;
    _config = config;
    _context = context;
    _pbuffer = pbuffer;
    _nvg = nvg;

    _fonts = new NanoVGFontProvider(nvg);

    if( ! initQuadRenderer())
    {
        PT_LOG_ERROR("initQuadRenderer failed to initialize GLES2 shaders");
    }

    PT_LOG_INFO("nanovg device created (EGL " << major << '.' << minor << ')');
    return true;
}


void NanoVGDevice::destroy()
{
    if(_fonts)
    {
        delete _fonts;
        _fonts = 0;
    }

    if(_quadProgram)
    {
        makeCurrentOffscreen();
        glDeleteProgram(_quadProgram);
        glDeleteBuffers(1, &_quadVbo);
        _quadProgram = 0;
        _quadVbo = 0;
    }

    // Destroy shared render-target FBO and stencil RBO while the context is
    // still current so the GL objects are released on the correct context.
    if(_renderFbo || _stencilRbo)
    {
        makeCurrentOffscreen();

        if(_stencilRbo)
        {
            glDeleteRenderbuffers(1, &_stencilRbo);
            _stencilRbo = 0;
        }

        if(_renderFbo)
        {
            glDeleteFramebuffers(1, &_renderFbo);
            _renderFbo = 0;
        }

        _rtW = 0;
        _rtH = 0;
    }

    EGLDisplay display = static_cast<EGLDisplay>(_display);

    if(_nvg)
    {
        eglMakeCurrent(display, static_cast<EGLSurface>(_pbuffer),
                       static_cast<EGLSurface>(_pbuffer),
                       static_cast<EGLContext>(_context));
        nvgDeleteGLES2(_nvg);
        _nvg = 0;
    }

    if(display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if(_pbuffer)
            eglDestroySurface(display, static_cast<EGLSurface>(_pbuffer));

        if(_context)
            eglDestroyContext(display, static_cast<EGLContext>(_context));

        eglTerminate(display);
    }

    _display = 0;
    _config = 0;
    _context = 0;
    _pbuffer = 0;
}


int NanoVGDevice::createImage(int w, int h)
{
    if( ! _nvg || w <= 0 || h <= 0)
        return -1;

    return nvgCreateImageRGBA(_nvg, w, h,
                              NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED,
                              0);
}


bool NanoVGDevice::bindRenderTarget(int nvgImage, int w, int h)
{
    if( ! _nvg || nvgImage < 0 || w <= 0 || h <= 0)
        return false;

    makeCurrentOffscreen();

    // Lazy-create the shared FBO on first use.
    if( ! _renderFbo)
    {
        glGenFramebuffers(1, &_renderFbo);
        glGenRenderbuffers(1, &_stencilRbo);
    }

    // Attach the pixmap texture as color attachment.
    GLuint tex = nvglImageHandleGLES2(_nvg, nvgImage);
    glBindFramebuffer(GL_FRAMEBUFFER, _renderFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, tex, 0);

    // Resize stencil RBO only when the target size changes.
    if(w != _rtW || h != _rtH)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, _stencilRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, _stencilRbo);
        _rtW = w;
        _rtH = h;
    }

    // Validate; if not complete try the depth/stencil packed fallback once.
    // GL_DEPTH24_STENCIL8 / GL_DEPTH_STENCIL_ATTACHMENT are not available in
    // pure GLES2 (they come from GL_OES_packed_depth_stencil). Only attempt
    // the plain stencil path; if the driver rejects it, log and bail.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        PT_LOG_ERROR("bindRenderTarget: FBO incomplete for "
                     << w << 'x' << h);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glViewport(0, 0, w, h);
    return true;
}


void NanoVGDevice::unbindRenderTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void NanoVGDevice::makeCurrentOffscreen()
{
    if( ! _display)
        return;

    eglMakeCurrent(static_cast<EGLDisplay>(_display),
                   static_cast<EGLSurface>(_pbuffer),
                   static_cast<EGLSurface>(_pbuffer),
                   static_cast<EGLContext>(_context));
}


void NanoVGDevice::makeCurrent(void* eglSurface)
{
    if( ! _display)
        return;

    eglMakeCurrent(static_cast<EGLDisplay>(_display),
                   static_cast<EGLSurface>(eglSurface),
                   static_cast<EGLSurface>(eglSurface),
                   static_cast<EGLContext>(_context));
}


int NanoVGDevice::fontFace(const Gfx::Font& font)
{
    if( ! _fonts)
        return -1;

    return _fonts->fontFace(font);
}


float NanoVGDevice::fontSizeScale(int handle)
{
    if( ! _fonts)
        return 1.0f;

    return _fonts->sizeScale(handle);
}


float NanoVGDevice::fontAscenderRatio(int handle)
{
    if( ! _fonts)
        return 0.9f;

    return _fonts->ascenderRatio(handle);
}


float NanoVGDevice::fontCapHeightRatio(int handle)
{
    if( ! _fonts)
        return 0.7f;

    return _fonts->capHeightRatio(handle);
}


float NanoVGDevice::fontXHeightRatio(int handle)
{
    if( ! _fonts)
        return 0.54f;

    return _fonts->xHeightRatio(handle);
}

static const char* quadVertexShader = R"(
    attribute vec2 pos;
    varying vec2 uv;
    void main() {
        uv = pos * 0.5 + 0.5;
        gl_Position = vec4(pos, 0.0, 1.0);
    }
)";

static const char* quadFragmentShader = R"(
    precision mediump float;
    uniform sampler2D tex;
    varying vec2 uv;
    void main() {
        gl_FragColor = texture2D(tex, uv);
    }
)";

bool NanoVGDevice::initQuadRenderer()
{
    if (_quadProgram != 0)
        return true;

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &quadVertexShader, nullptr);
    glCompileShader(vShader);
    
    int success = 0;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        PT_LOG_ERROR("Quad vertex shader compile failed");
        return false;
    }

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &quadFragmentShader, nullptr);
    glCompileShader(fShader);
    
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        PT_LOG_ERROR("Quad fragment shader compile failed");
        return false;
    }

    _quadProgram = glCreateProgram();
    glAttachShader(_quadProgram, vShader);
    glAttachShader(_quadProgram, fShader);
    glLinkProgram(_quadProgram);

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    
    _quadPosAttr = glGetAttribLocation(_quadProgram, "pos");
    _quadTexUniform = glGetUniformLocation(_quadProgram, "tex");

    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glGenBuffers(1, &_quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    return true;
}

void NanoVGDevice::renderTexturedQuad(int nvgImage)
{
    if (!initQuadRenderer())
        return;

    GLuint tex = nvglImageHandleGLES2(_nvg, nvgImage);
    if (tex == 0)
        return;

    GLint prevProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

    glUseProgram(_quadProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    if (_quadTexUniform >= 0)
        glUniform1i(_quadTexUniform, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
    if (_quadPosAttr >= 0) {
        glVertexAttribPointer(_quadPosAttr, 2, GL_FLOAT, GL_FALSE, 8, nullptr);
        glEnableVertexAttribArray(_quadPosAttr);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (_quadPosAttr >= 0) {
        glDisableVertexAttribArray(_quadPosAttr);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (prevProgram > 0)
        glUseProgram(prevProgram);
}

} // namespace Forms

} // namespace Pt
