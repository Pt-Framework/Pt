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

#ifndef Pt_Forms_Texture_H
#define Pt_Forms_Texture_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <string>
#include <vector>

#include "vector.h"

namespace Pt {

namespace Forms {

class Texture 
{
public:
  Texture(int width, int height);
  ~Texture();

  void bind();
  void bind(GLuint slot);
  
  void unbind();
  void unbind(GLuint slot);

  GLuint textureID() const
  { return _texID; }

  int width() const
  { return _width; }

  int height() const
  { return _height; }

  std::vector<GLubyte>& data()
  { return _data; }

  void setData(const std::vector<GLubyte>& data, int w, int h);

private:
  void Create();
  Texture(const Texture& );
  Texture& operator = (const Texture&);

private:
  int                   _width;
  int                   _height;
  GLuint                _texID;
  std::vector<GLubyte>  _data;
  bool                  _bound;
};

} // namespace Forms

} // namespace Pt


#endif // include guard