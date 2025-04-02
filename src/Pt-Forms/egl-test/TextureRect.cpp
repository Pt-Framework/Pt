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

#include "TextureRect.h"

namespace {

const unsigned short indices[] = {
  0, 1, 2,   
  2, 1, 3,
};

}

namespace Pt {

namespace Forms {
  
TextureRect::TextureRect(float width, float height)
  : _width(width)
  , _height(height)
  , _indices(indices, indices + ( sizeof(indices) / sizeof(unsigned short) ) )
{
  generateVertices();
}


TextureRect::~TextureRect()
{
}

void TextureRect::generateVertices()
{
  std::vector<float> vertices;
  std::vector<float> texCoords;
  
  vertices.resize(4 * 3);
  texCoords.resize(4 * 3);

  float* rectVertices   = &vertices[0];
  float* rectTexCoords  = &texCoords[0];

  int i = 0;
  int t = 0;

  rectVertices[i++]   = 0.0;    rectVertices[i++]   = 0.0;      rectVertices[i++]   = 0.0;
  rectTexCoords[t++]  = 0.0;    rectTexCoords[t++]  = 1.0;      rectTexCoords[t++]  = 0.0;

  rectVertices[i++]   = 0.0;    rectVertices[i++]   = _height;  rectVertices[i++]   = 0.0;
  rectTexCoords[t++]  = 0.0;    rectTexCoords[t++]  = 0.0;      rectTexCoords[t++]  = 0.0;

  rectVertices[i++]   = _width; rectVertices[i++]   = 0.0;      rectVertices[i++]   = 0.0;
  rectTexCoords[t++]  = 1.0;    rectTexCoords[t++]  = 1.0;      rectTexCoords[t++]  = 0.0;

  rectVertices[i++]   = _width; rectVertices[i++]   = _height;  rectVertices[i++]   = 0.0;
  rectTexCoords[t++] = 1.0;     rectTexCoords[t++]  = 0.0;      rectTexCoords[t++]  = 0.0;

  this->setVertices(vertices);
  this->setTextureCoords(texCoords);
}

} // namespace Forms

} // namespace Pt
