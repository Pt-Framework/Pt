 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
    Copyright (C) 2015 Ilja Maier
  
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

#include "Display.h"

#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <fcntl.h>
#include <unistd.h>

PT_LOG_DEFINE("Pt.Hmi.Display")

namespace Pt {
namespace Hmi {

Display::Display()
  : _fd(-1)
  , _display(0)
  , _surface(0)
  , _width(0)
  , _height(0)
{
  System::Logger::setLogLevel("", System::Trace);
  init();
}


Display::~Display()
{
  eglDestroySurface(_display, _surface);
  eglDestroyContext(_display, _context);
  eglTerminate(_display);

  if( _fd != -1 )
    close(_fd);
}


void Display::init()
{
  PT_LOG_DEBUG("Display::init" );

  static const char *devices[] = { "vmwgfx", "i915", "radeon", "nouveau", "omapdrm" };
  for( int i = 0; i < 5; ++i ) 
  {
    _fd = drmOpen(devices[i], NULL);
    if( _fd >= 0 )
      break;
  }

  if( _fd < 0 ) 
  {
    _fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
  }
  if( _fd < 0 )
    throw System::AccessFailed("/dev/dri/card0");

  drmModeRes* resources = drmModeGetResources(_fd);
  if( ! resources )
    throw std::runtime_error("drmModeGetResources failed");

  drmModeConnector* connector = 0;
  for( int i = 0; i < resources->count_connectors; ++i ) 
  {
    connector = drmModeGetConnector(_fd, resources->connectors[i]);
    if(  connector->connection == DRM_MODE_CONNECTED
      || connector->connection == DRM_MODE_UNKNOWNCONNECTION ) // For Devkit
      break;
    drmModeFreeConnector(connector);
    connector = 0;
  }
  if( ! connector ) 
  {
    drmModeFreeResources(resources);
    throw std::runtime_error("Could not get DRM connector.");
  }

  drmModeModeInfo* mode = 0;
  for( int i = 0; i < connector->count_modes; ++i ) 
  {
    if( connector->modes[i].type & DRM_MODE_TYPE_PREFERRED ) 
    {
      mode = &connector->modes[i];
      break;
    }
  }
  if( ! mode ) 
  {
    drmModeFreeConnector(connector);
    drmModeFreeResources(resources);
    throw std::runtime_error("Could not get DRM mode.");
  }

  Pt::uint32_t crtc = 0;
  for( int i = 0; i < connector->count_encoders; ++i) 
  {
    drmModeEncoder* encoder = drmModeGetEncoder(_fd, connector->encoders[i]);
    if( ! encoder )
      continue;

    // Break after first compatible CRTC
    for( int j = 0; j < resources->count_crtcs; ++j ) 
    {
      if( (encoder->possible_crtcs & (1 << j)) ) 
      {
        crtc = resources->crtcs[j];
        break;
      }
    }
    if( crtc )
      break;
  }
  
  if( ! crtc )
    throw std::runtime_error("Could not determine CRTC.");

  gbm_device* device = gbm_create_device(_fd);
  if( ! device )
    throw std::runtime_error("Could not determine CRTC.");

  gbm_surface* gbm = gbm_surface_create(device, mode->hdisplay, mode->vdisplay,
    GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
  if( ! gbm )
    throw std::runtime_error("Could not initialize GBM surface.");

  // Create EGLDisplay from device
  _display = eglGetDisplay( (EGLNativeDisplayType) device);
  if( ! eglInitialize(_display, 0, 0) )
    throw std::runtime_error("Could not initialize EGL display.");

  PT_LOG_DEBUG("eglGetDisplay" );
  
  EGLint attr[] = 
  {   
    EGL_RED_SIZE,         8,
    EGL_GREEN_SIZE,       8,
    EGL_BLUE_SIZE,        8,
    EGL_ALPHA_SIZE,       8,
    EGL_DEPTH_SIZE,       16, 
    EGL_RENDERABLE_TYPE,  EGL_OPENGL_ES2_BIT, 
    EGL_SURFACE_TYPE,     EGL_PBUFFER_BIT, // pbuffer
    EGL_NONE
  };

  EGLConfig   config;
  EGLint      num_config;

  if( ! eglChooseConfig( _display, attr, &config, 1, &num_config ) ) 
    throw std::runtime_error("Failed to choose config");

  if( num_config != 1 )
    throw std::runtime_error("Didn't get exactly one config");

    /*_surface = eglCreateWindowSurface(_display, config, (EGLNativeWindowType)gbm, 0);
    if( _surface == EGL_NO_SURFACE )
      throw std::runtime_error("Could not create EGL window surface.");*/

   	EGLint attrib_list[] = { EGL_HEIGHT,          256,
                             EGL_WIDTH,           256,
                             EGL_LARGEST_PBUFFER, EGL_TRUE,
                             EGL_NONE 
                            };

    _surface = eglCreatePbufferSurface(_display, config, attrib_list);
    if( _surface == EGL_NO_SURFACE )
      throw std::runtime_error("Could not create EGL window surface.");


    EGLint width;
    EGLint height;
    if ( ! eglQuerySurface ( _display, _surface, EGL_WIDTH, &width ) ||
         ! eglQuerySurface ( _display, _surface, EGL_HEIGHT, &height ))
    {
      PT_LOG_DEBUG("eglQuerySurface failed");
    }

    PT_LOG_DEBUG("Pbuffer width: " << width << " height:" << height);


    EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttributes);
    if( ! _context )
      throw std::runtime_error("Could not create EGL context." );

    if( ! eglMakeCurrent(_display, _surface, _surface, _context) )
      throw std::runtime_error("Could not set the current EGL context.");

    if( ! eglSwapBuffers(_display, _surface) )
      throw std::runtime_error("Could not perform initial buffer swap.");

  // Lock front buffer
  gbm_bo* bo = gbm_surface_lock_front_buffer(gbm);
  if( ! bo )
    throw std::runtime_error("Could not lock the front buffer.");

  // Add first frame buffer & attach id
  Pt::uint32_t id;
  if( drmModeAddFB(_fd, gbm_bo_get_width(bo), gbm_bo_get_height(bo),
    24, 32, gbm_bo_get_stride(bo), gbm_bo_get_handle(bo).u32, &id) )
    throw std::runtime_error("Could not add DRM framebuffer.");

  // Set mode
  if( drmModeSetCrtc(_fd, crtc, id, 0, 0, &connector->connector_id, 1, mode))
    throw std::runtime_error("Could not set DRM mode.");

  _width  = mode->hdisplay;
  _height = mode->vdisplay;
  
  PT_LOG_DEBUG("_width: " << _width << " height:" << _height);
  PT_LOG_DEBUG("_width: " << this->width() << " height:" << this->height());

  glViewport(0, 0, _width, _height);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}


} // namespace Hmi
} // namespace Pt
