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

#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include "PaintSurfaceImpl.h"
#include "Display.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>
#include <algorithm>


PT_LOG_DEFINE("Pt.Hmi.Screen")

namespace Pt {

namespace Hmi {
  
ScreenImpl::ScreenImpl(ApplicationImpl& app)
: _dpi(96.0)
, _display( app.display() )
//, _winSurface( app.display().surface() )
{
  app.eventReady() += Pt::slot(_eventReceived);

  Size.set( Gfx::SizeF( app.display().width(), app.display().height() ) ); 

  PT_LOG_DEBUG("Screen size: " << app.display().width() << " x " << app.display().height() );

  BackColor.set( Gfx::Color(170/255.0f, 170/255.0f, 170/255.0f) );
  Visible.set(true);      
  setCursor(0);  
  
  eventReceived() += Pt::slot( *this, &ScreenImpl::onPointerInput );


  _fbo = 0;
   _texture = 0;
   _program = 0;

   initFBO();
}


ScreenImpl::~ScreenImpl()
{
}


void ScreenImpl::onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent )
{    
  //Pt::System::Clock clock;
  //clock.start();

  //_drawCursor =  true;

  //if( !_cursorBackground.empty() )
  //  bitBlit( _cursorBackground.pixel(0,0), _cursorBackground.width(), _cursorBackground.height(), _cursorPos, (Pt::uint8_t*)  _image.pixel(0,0), CopyOp );

  //if( Cursor.get().width() != 0 )
  //  _cursorPos = Gfx::Point( mouseEvent.x() - Cursor.get().xHotspot() , mouseEvent.y() - Cursor.get().yHotspot());

  _windowManager.pointerInput( mouseEvent );    
  
  //if( _drawCursor )
  //  updateScreen();

  //std::clog << "screen update: " << clock.stop().toUSecs() / 1000.0 << " msecs" << std::endl;
}


void ScreenImpl::onInvalidate()
{
  Window::render();
}



GLuint ScreenImpl::LoadShader( GLenum type, const char *shaderSrc )
{
  GLuint shader;
  GLint compiled;

  shader = glCreateShader( type );

  if( shader == 0 )
    return 0;

  glShaderSource( shader, 1, &shaderSrc, NULL );
  
  glCompileShader( shader );

  glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

  if( ! compiled ) 
  {
    GLint infoLen = 0;

    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );
    if( infoLen > 1 )
    {
      // error
    }

    glDeleteShader ( shader );
    return 0;
  }

  return shader;
}



void ScreenImpl::initFBO()
{
   char vShaderStr[] =  
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";
   
   char fShaderStr[] =  
      "precision mediump float;                            \n"
      "varying vec2 v_texCoord;                            \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
      "}                                                   \n";

  //glGenFramebuffers(1, &_fbo);
  //glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  //glGenTextures(1, &_texture);
  //glBindTexture(GL_TEXTURE_2D, _texture);
  //glTexImage2D( GL_TEXTURE_2D,
  //              0,
  //              GL_RGBA,
  //              300, 300,
  //              0,
  //              GL_RGBA,
  //              GL_UNSIGNED_BYTE,
  //              NULL);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
 
  //// FBO status check
  //GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  //switch(status) 
  //{
  //case GL_FRAMEBUFFER_COMPLETE:
  //  break;

  //case GL_FRAMEBUFFER_UNSUPPORTED:
  //  break;

  //default:
  //  break;
  //}

  //glBindFramebuffer(GL_FRAMEBUFFER, 0);



   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[44 * 3] =
   {  
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0,  // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0 , // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0,  // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0 , // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0 , // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0 , // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0 , // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0,  // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0,  // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0,  // Yellow

      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &_texture );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, _texture );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 5, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  GLuint vertexShader;
  GLuint fragmentShader;

  vertexShader = LoadShader( GL_VERTEX_SHADER, vShaderStr );
  fragmentShader = LoadShader( GL_FRAGMENT_SHADER, fShaderStr );

  _program = glCreateProgram();
  if( _program == 0 )
    return;

  glAttachShader( _program, vertexShader );
  glAttachShader( _program, fragmentShader );

  positionLoc = glGetAttribLocation ( _program, "a_position" );
  texCoordLoc = glGetAttribLocation ( _program, "a_texCoord" );

  samplerLoc = glGetUniformLocation ( _program, "s_texture" );
}


void ScreenImpl::onRender(PaintSurface& surface)
{
  PT_LOG_DEBUG("++++++++++ RENDERING SCREEN ++++++++++");
  PT_LOG_DEBUG("ScreenImpl::onRender: " << _display.width() 
                                 << ' ' << _display.height() );

  glViewport(0, 0, surface.size().width(), surface.size().height());

  GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0 
                           -0.5f, -0.5f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            0.5f, -0.5f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            0.5f,  0.5f, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                         };
   GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


   glUseProgram ( _program );

   // Load the vertex position
   glVertexAttribPointer ( positionLoc, 3, GL_FLOAT, 
                           GL_FALSE, 5 * sizeof(GLfloat), vVertices );
   // Load the texture coordinate
   glVertexAttribPointer ( texCoordLoc, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

   glEnableVertexAttribArray ( positionLoc );
   glEnableVertexAttribArray ( texCoordLoc );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, _texture);

   // Set the sampler texture unit to 0
   glUniform1i ( samplerLoc, 0 );

   glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );


 /* glBindTexture(GL_TEXTURE_2D, _texture);
  glEnable(GL_TEXTURE_2D);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glViewport(0,0, 300, 300);
  glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);*/



  
    // render main
  /*glClearColor(0.f, 0.f, 1.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);*/










  //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //glClear(GL_COLOR_BUFFER_BIT);


  //glBindFramebuffer(GL_FRAMEBUFFER, 0);


  //GLuint rboId;
  //glGenRenderbuffers(1, &rboId);
  //PT_LOG_DEBUG("glGenRenderbuffers: " << rboId );

  //glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 300, 300);
  //glBindFramebuffer(GL_RENDERBUFFER, 0);
  //
  //GLuint fboId;
  //glGenFramebuffers(1, &fboId);
  //glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  //glFramebufferRenderbuffer(GL_FRAMEBUFFER,
  //                          GL_COLOR_ATTACHMENT0,
  //                          GL_RENDERBUFFER,
  //                          rboId);

  //GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  //PT_LOG_DEBUG("glCheckFramebufferStatus: " << (status != GL_FRAMEBUFFER_COMPLETE) );

  //glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
  //glClear(GL_COLOR_BUFFER_BIT);
  ////glBindFramebuffer(GL_FRAMEBUFFER, 0);

  
  
  
  
  _display.updateScreen();

  // switch back to window-system-provided framebuffer
  

  //Window::onRender(surface);

  // surface contains whole screen image now...
  PT_LOG_DEBUG("########## BLIT SURFACE TO DISPLAY ##########");

  eglSwapBuffers(_display.display(), _display.surface() );
}


void ScreenImpl::setCursor( const Hmi::Cursor* cursor )
{    
  Cursor = (cursor == 0 ? Hmi::Cursor::defaultCursor() : *cursor );    
}

}

}
