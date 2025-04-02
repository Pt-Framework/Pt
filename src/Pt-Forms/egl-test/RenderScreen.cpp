 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
    Copyright (C) 2016 Ilja Maier
  
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

#include "RenderScreen.h"
#include "Shaders.h"
#include "Vector.h"
#include "Matrix.h"

#include <iostream>
#include <cstdlib>


namespace Pt {

namespace Forms {

int RandomBetween(int a, int b)
{
  return rand() % ( b - a + 1 ) + a;
}

void makeNoise(std::vector<GLubyte>& data, int width, int height)
{
  for( int i = 0; i < width * height * 4; i +=4 )
  {
    data[i]   = RandomBetween(0, 255); // red
    data[i+1] = RandomBetween(0, 255); // green
    data[i+2] = RandomBetween(0, 255); // blue
    data[i+3] = 255;                   // alpha channel
  }
}


void makeSolidColor(std::vector<GLubyte>& data, ivec4 color, int width, int height)
{
  for( int i = 0; i < width * height * 4; i +=4 )
  {
    data[i]   = color.r; // red
    data[i+1] = color.g; // green
    data[i+2] = color.b; // blue
    data[i+3] = color.a; // alpha channel
  }
}




RenderScreen::RenderScreen(int width, int height)
  : _width(width)
  , _height(height)
  , _rectNoiseTexture(50.f, 50.f)
  , _rectSolidTexture(100.f, 100.f)
  , _rectSolidColor(20.f, 10.f, vec4(0.f, 1.f, 0.f, 0.25f))
{
  Init();
}


RenderScreen::~RenderScreen()
{
  Cleanup();
}


void RenderScreen::Init()
{
  // test transformations 
  _rectNoiseTexture.scale(10.f);
  _rectNoiseTexture.rotate(45.f);
  _rectNoiseTexture.translate( vec2( 0.f, 10.f ));
  
  _rectSolidTexture.translate( vec2( 200.f, 20.f ));
  
  _rectSolidColor.scale(15.f);
  _rectSolidColor.translate( vec2( 30.f, 20.f ));  

  // -------------  create texures for test
  std::vector<GLubyte> data( 256 * 256 * 4 * sizeof(GLubyte), 0x00000000);
  makeNoise(data, 256, 256);
  Texture* tex1 = new Texture(256, 256);
  tex1->setData(data, 256, 256);

  ivec4 color;
  color.r = 255; color.g = 255;
  color.b = 0; color.a = 50;

  std::vector<GLubyte> data2( 256 * 300 * 4 * sizeof(GLubyte), 0x00000000);
  makeSolidColor(data2, color, 256, 300);
  Texture* tex2 = new Texture(256, 300);
  tex2->setData(data2, 256, 300);

  _textures.insert(std::pair<std::string, Texture*>("noiseTexture", tex1));
  _textures.insert(std::pair<std::string, Texture*>("solidTexture", tex2));

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport( 0, 0, _width, _height );
}


void RenderScreen::Cleanup()
{
  std::map<std::string, Texture*>::iterator it;   
  for( it = _textures.begin(); it != _textures.end(); ++it ) 
  {
    Texture* t = it->second;  
    delete t;
  }
  
  glFinish();
}


void RenderScreen::Render()
{
  glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Matrix orthoProj(Matrix::ortho(0.0f, static_cast<float>(_width), 
    static_cast<float>(_height), 0.0f , -1.0f, 1.0f));

  // -------------- texture test -------------------------------//
  ShaderProgram textureProgram(texture_vsh, texture_fsh);
  textureProgram.bind();
  {
    glUniformMatrix4fv( textureProgram.uniform("projection"),1, GL_FALSE,  orthoProj.get() );
    glUniformMatrix4fv( textureProgram.uniform("model"), 1, GL_FALSE, _rectNoiseTexture.getModelView().get() );

    glUniform1i( textureProgram.uniform("s_tex"), 0);
    Texture* tex1 = _textures["noiseTexture"];
    tex1->bind();
    _rectNoiseTexture.draw( textureProgram, GL_TRIANGLES, _rectNoiseTexture.Indices() );
    tex1->unbind();

    glUniformMatrix4fv( textureProgram.uniform("model"), 1, GL_FALSE, _rectSolidTexture.getModelView().get() );
    glUniform1i( textureProgram.uniform("s_tex"), 0);
    Texture* tex2 = _textures["solidTexture"];
    tex2->bind();
    _rectSolidTexture.draw( textureProgram, GL_TRIANGLES, _rectSolidTexture.Indices() );
    tex2->unbind();
  } 
  textureProgram.unbind();

  // -------------- solid color test -------------------------------//
  //vec4 color (1.0f, 0.0f, 0.0f, 1.0f);
  ShaderProgram colorProgram(color_vsh, color_fsh);
  colorProgram.bind();
  {
    //_rectSolidColor.rotate(45.f);

    glUniformMatrix4fv( colorProgram.uniform("projection"),1, GL_FALSE,  orthoProj._m );
    glUniformMatrix4fv( colorProgram.uniform("model"), 1, GL_FALSE, _rectSolidColor.getModelView().get() );
    glUniform4fv( colorProgram.uniform("color"), 1, &_rectSolidColor.Color().x);
    _rectSolidColor.draw( colorProgram, GL_TRIANGLES, _rectSolidColor.Indices() );
  }
  colorProgram.unbind();
}


} // namespace Forms

} // namespace Pt
