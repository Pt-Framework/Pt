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

#ifndef Pt_Forms_Shape_H
#define Pt_Forms_Shape_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <vector>

#include "ShaderProgram.h"
#include "Matrix.h"

namespace Pt {

namespace Forms {

class Shape
{
  public:
    Shape();
    
    virtual ~Shape();

    void draw(const ShaderProgram& program, GLenum mode, const unsigned short* indices);

    const Matrix& getModelView() const
    { return _modelView; }

    void scale( float factor);
    void translate( const vec2& position );
    void rotate( float deg );
    void reset();

    void setVertices(const std::vector<float>& vertices)
    { _vertices = vertices; }

    void setTextureCoords(const std::vector<float>& texCoords)
    { _texCoords = texCoords; }

  private:
    virtual void generateVertices() = 0;
   
  private:

    // TODO: add return multiplication modelviewMatrix = scale * rotation * translation
    Matrix _modelView; 

    std::vector<float> _vertices;
    std::vector<float> _texCoords;
};

} // namespace Forms

} // namespace Pt

#endif // include guard
