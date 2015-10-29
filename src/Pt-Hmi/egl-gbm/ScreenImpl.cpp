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
#include "ShaderProgram.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Logger.h>

#include <algorithm>
#include <cstdlib> // rand()
#include <ctime>


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

  _fboA = 0;
  _fboB = 0;
  _depthBuf = 0;
  _textureA = 0;
  _textureB = 0;
  _mainProgram = 0;
  _textProgram = 0;

  _counter = 0;

  _positionLoc = 0;
  _texCoordLoc = 0;
  _samplerLoc = 0;
  _texColor = 0;

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
  char vShader[] =  
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "attribute vec4 a_Color;      \n"
      "varying vec4 v_Color;        \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "   v_Color = a_Color;        \n"
      "}                            \n";

   
   char fShader[] =  
      "precision mediump float;                            \n"
      "varying vec2 v_texCoord;                            \n"
      "varying vec4 v_Color;                               \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "   gl_FragColor = texture2D( s_texture, v_texCoord ) * v_Color; \n"
      "}\n";

    char vShaderTexture[] =
    "attribute vec4 a_position; \n"
    "attribute vec2 a_texCoord; \n"
    "varying vec2 v_texCoord; \n"
    "void main() { \n"
    "  gl_Position = a_position; \n"
    "  v_texCoord = a_texCoord; \n"
    "} \n";

   char fShaderTexture[] =
    "precision mediump float;  \n"                          
    "bvarying vec2 v_texCoord; \n"                           
    "buniform sampler2D tex;  \n"                     
    "void main() { \n" 
    "  gl_FragColor = texture2D( tex, v_texCoord ); \n"
    "}      \n";

  // shader
  GLuint vertexShader;
  GLuint fragmentShader;

  vertexShader = LoadShader( GL_VERTEX_SHADER, vShader );
  fragmentShader = LoadShader( GL_FRAGMENT_SHADER, fShader );

  _mainProgram = glCreateProgram();
  if( _mainProgram == 0 )
    return;

  glAttachShader( _mainProgram, vertexShader );
  glAttachShader( _mainProgram, fragmentShader );

  glLinkProgram( _mainProgram );
  GLint linked;
  glGetProgramiv( _mainProgram, GL_LINK_STATUS, &linked );
  if( ! linked ) 
  {
    GLint infoLen = 0;

    glGetProgramiv( _mainProgram, GL_INFO_LOG_LENGTH, &infoLen );

    if( infoLen > 1 )
    {
      std::cerr << " linked failed" << std::endl;
    }

    glDeleteProgram( _mainProgram );
    return;
  }

  GLint maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  PT_LOG_DEBUG("GL_MAX_TEXTURE_SIZE: " << maxTextureSize);
  PT_LOG_DEBUG("OpenGL VERSION: " << glGetString(GL_VERSION));
  PT_LOG_DEBUG("OpenGL GL_EXTENSIONS " << glGetString(GL_EXTENSIONS));

  //create texture A
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &_textureA);
  glBindTexture(GL_TEXTURE_2D, _textureA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  // generate texture data
  std::vector<GLubyte> data(256 * 256 * 4 * sizeof(GLubyte), 0xFFFF00FF);
  GLubyte val = 0;
  for( int i = 0; i < 256*256*4; i += 4 )
  {
    if( std::rand( /*std::time(0)*/ ) / RAND_MAX > 0.5 )
      val = 0;
    else
      val = 255;

    data[i] = data[i+1] = data[i+2] = val;
    data[i+3] = 255;
  }

  //create texture B
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &_textureB);
  glBindTexture(GL_TEXTURE_2D, _textureB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

  //create fboA and attach texture A to it
  glGenFramebuffers(1, &_fboA);
  glBindFramebuffer(GL_FRAMEBUFFER, _fboA);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureA, 0);

  //create fboB and attach texture B to it
  glGenFramebuffers(1, &_fboB);
  glBindFramebuffer(GL_FRAMEBUFFER, _fboB);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureB, 0);

}


void ScreenImpl::onRender(PaintSurface& surface)
{
  PT_LOG_DEBUG("++++++++++ RENDERING SCREEN ++++++++++");
  PT_LOG_DEBUG("ScreenImpl::onRender: " << _display.width() 
                                 << ' ' << _display.height() );
  _counter++;

   _display.updateScreen();
}


void ScreenImpl::RenderTextureToScreen(GLuint textId)
{
}


void ScreenImpl::setCursor( const Hmi::Cursor* cursor )
{    
  Cursor = (cursor == 0 ? Hmi::Cursor::defaultCursor() : *cursor );    
}

}

}
