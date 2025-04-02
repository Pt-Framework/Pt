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

#include "Shape.h"

namespace Pt {

namespace Forms {
  
Shape::Shape()
  : _modelView()
{
}

Shape::~Shape()
{
}


void Shape::draw(const ShaderProgram& program, GLenum mode, const unsigned short* indices) 
{
  if ( _vertices.empty() )
    return;

  glVertexAttribPointer( program.attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, &_vertices[0] );
  glEnableVertexAttribArray( program.attribute("position") );

  if( ! _texCoords.empty() )
  {
    glVertexAttribPointer( program.attribute("texCoord"), 3, GL_FLOAT, GL_FALSE, 0, &_texCoords[0] );
    glEnableVertexAttribArray( program.attribute("texCoord") );
  }

  if( mode == GL_TRIANGLES ) 
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
  else if( mode == GL_LINES ) 
    glDrawElements( GL_LINES, 8, GL_UNSIGNED_SHORT, indices );

  glDisableVertexAttribArray( program.attribute("position") );

  if( ! _texCoords.empty() )
    glDisableVertexAttribArray( program.attribute("texCoord") );
}


void Shape::scale( float factor)
{
  _modelView.scale( vec3(factor, factor, 0.0) );
}

void Shape::translate( const vec2& position )
{
  _modelView.translate( vec3( position.x, position.y, 0) );
}

void Shape::rotate( float deg )
{
  _modelView.rotateX( deg );
  _modelView.rotateY( deg );
}

void Shape::reset()
{
  _modelView.identity();
}

} // namespace Forms

} // namespace Pt
