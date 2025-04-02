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

#include "ShaderProgram.h"

#include <stdexcept>

namespace Pt {

namespace Forms {

ShaderProgram::ShaderProgram(const std::string& vsh, const std::string& fsh)
  : _programID(0)
  , _vertID(0)
  , _fragID(0)
{
  _programID = glCreateProgram();

  _vertID = compile( GL_VERTEX_SHADER,    vsh );
  _fragID = compile( GL_FRAGMENT_SHADER,  fsh );

  glAttachShader(_programID, _vertID);
  glAttachShader(_programID, _fragID);
  glLinkProgram(_programID);

  GLint result = GL_FALSE;
  glGetProgramiv(_programID, GL_LINK_STATUS, &result);
  if( result != GL_TRUE )
  {
    GLsizei logLength = 0;
    GLchar logBuffer[1024];
    glGetShaderInfoLog(_programID, 1024, &logLength, logBuffer);

    std::string msg(logBuffer, logLength);
    throw std::runtime_error(msg);
  }

  if(_vertID)
    glDeleteShader(_vertID);

  if(_fragID)
    glDeleteShader(_fragID);

  getAttributes();
  getUniforms();
}


ShaderProgram::~ShaderProgram()
{
  if( _programID ) 
    glDeleteProgram(_programID);
}


void ShaderProgram::bind()
{
  glUseProgram(_programID);
}


void ShaderProgram::unbind()
{
  glUseProgram(0);
}


GLuint ShaderProgram::compile(GLenum type, const std::string& source)
{
  const GLchar* shaderSource = source.c_str();

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  GLint result = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if(result != GL_TRUE) 
  {
    GLsizei logLength = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shader, 1024, &logLength, message);

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}



GLuint ShaderProgram::uniform(const std::string& name) const
{
  std::map<std::string, GLuint>::const_iterator it = _uniforms.find(name);
  if(it == _uniforms.end())
    return 0;

  return it->second;
}


GLuint ShaderProgram::attribute(const std::string& name) const
{
  std::map<std::string, GLuint>::const_iterator it = _attributes.find(name);
  if(it == _attributes.end())
    return 0;

  return it->second;
}


void ShaderProgram::getAttributes()
{
  GLint count = 0;
  glGetProgramiv(_programID, GL_ACTIVE_ATTRIBUTES, &count);

  GLchar buffer[256];
  GLsizei nameSize = 0;
  GLint attrSize = 0;
  GLenum attrType = 0;

  for(GLint i = 0; i < count; i++)
  {
    glGetActiveAttrib(_programID, i, sizeof(buffer), &nameSize, &attrSize, &attrType, buffer);

    std::string name(buffer, nameSize);
    _attributes[name] = glGetAttribLocation(_programID, name.c_str());
  }
}
  

void ShaderProgram::getUniforms()
{
  GLint count = 0;
  glGetProgramiv(_programID, GL_ACTIVE_UNIFORMS, &count);

  GLchar buffer[256];
  GLsizei nameSize = 0;
  GLint attrSize = 0;
  GLenum attrType = 0;

  for(GLint i = 0; i < count; i++)
  {
    glGetActiveUniform(_programID, i, sizeof(buffer), &nameSize, &attrSize, &attrType, buffer);
    
    std::string name(buffer, nameSize);
    _uniforms[name] = glGetUniformLocation(_programID, name.c_str());
  }
}

} // namespace Forms

} // namespace Pt
