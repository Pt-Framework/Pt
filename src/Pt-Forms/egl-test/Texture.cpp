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

#include "Texture.h"

namespace Pt {

namespace Forms {

Texture::Texture(int width, int height)
  : _width(width)
  , _height(height)
  , _texID(0)
  , _data( _width * _height * 4 * sizeof(GLubyte), 0xFF0000FF )
  , _bound(false)
{
  Create();
}

Texture::~Texture()
{
  unbind();
  glDeleteTextures(1, &_texID); 
}


void Texture::Create() 
{
  glGenTextures(1, &_texID);
  glBindTexture(GL_TEXTURE_2D, _texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Texture::bind()
{
  glBindTexture(GL_TEXTURE_2D, _texID);
  _bound = true;
}

void Texture::bind(GLuint slot)
{
  glBindTexture(GL_TEXTURE_2D, _texID);
  _bound = true;
}


void Texture::unbind()
{
  if( _bound )
  {
    glBindTexture(GL_TEXTURE_2D, 0);
    _bound = false;
  }
}

void Texture::unbind(GLuint slot)
{
  if( _bound )
  {
    glBindTexture(GL_TEXTURE_2D, 0);
    _bound = false;
  }
}

void Texture::setData(const std::vector<GLubyte>& data, int w, int h)
{
  _width = w;
  _height = h;
  _data = data;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &_data[0]);
}

} // namespace Forms

} // namespace Pt
