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

#include <Pt/System/Logger.h>

#include <fstream>
#include <iostream>
#include <vector>

PT_LOG_DEFINE("Pt.Hmi.ShaderProgram")

namespace Pt {

namespace Hmi {
  
ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
  : _programID(-1)
  , _vertID(-1)
  , _fragID(-1)
{
  compile(vertexPath, fragmentPath);
  install();
}


ShaderProgram::~ShaderProgram()
{
}


std::string ShaderProgram::readFile(const std::string& filePath)
{
  std::string content;
  std::ifstream fileStream(filePath.c_str(), std::ios::in);

  if( ! fileStream.is_open() ) 
  {
    std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
    return "";
  }

  std::string line = "";
  while( !fileStream.eof() ) 
  {
    std::getline(fileStream, line);
    content.append(line + '\n');
  }

  fileStream.close();
  return content;
}


void ShaderProgram::compile(const std::string& vertexPath, const std::string& fragmentPath)
{
  _programID = glCreateProgram();

  _vertID = compile( GL_VERTEX_SHADER,    readFile(vertexPath) );
  _fragID = compile( GL_FRAGMENT_SHADER,  readFile(fragmentPath) );

}


GLuint ShaderProgram::compile(GLenum type, const std::string& source)
{
  if( source.empty() )
    return -1;

  const GLchar* shaderSource = source.c_str();

  GLuint shader = 0;
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  GLint result = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if( result != GL_TRUE ) 
  {
    GLsizei logLength = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shader, 1024, &logLength, message);
    
    PT_LOG_DEBUG("Failed to compile shader" << message);

    glDeleteShader(shader);
  }

  return shader;
}


void ShaderProgram::link(GLuint progID)
{
  glLinkProgram(progID);

  GLint result = GL_FALSE;
  glGetShaderiv(progID, GL_LINK_STATUS, &result);
  if( result != GL_TRUE )
  {
    GLsizei logLength = 0;
    GLchar message[1024];

    glGetShaderInfoLog(progID, 1024, &logLength, message);   
    PT_LOG_DEBUG("Failed to link shader" << message);

    if( _vertID ) 
    {
      glDeleteShader(_vertID);
      _vertID = 0;
    }
    if( _fragID ) 
    {
      glDeleteShader(_fragID);
      _fragID = 0;
    }
    if( _programID ) 
    {
      glDeleteProgram(_programID);
      _programID = 0;
    }
  }

}


void ShaderProgram::install()
{
  glAttachShader(_programID, _vertID);
  glAttachShader(_programID, _fragID);

  link(_programID);

  mapAttributs();
  mapUniforms();

  // release vertex and fragment shaders.
  if(_vertID)
    glDeleteShader(_vertID);

  if(_fragID)
    glDeleteShader(_fragID);
}

void ShaderProgram::bind()
{
  glUseProgram(_programID);
}

void ShaderProgram::unbind()
{
  glUseProgram(0);
}


GLuint ShaderProgram::uniform(const std::string& name)
{
  return _uniforms[name];
}

GLuint ShaderProgram::attribute(const std::string& name)
{
  return _attributes[name];
}


void ShaderProgram::mapAttributs()
{
  GLint count = 0;
  glGetProgramiv(_programID, GL_ACTIVE_ATTRIBUTES, &count);

  GLchar name[256];
  GLsizei length = 0;
  GLint size = 0;
  GLenum type;

  for(int i = 0; i < count; i++)
  {
    glGetActiveAttrib(_programID, i, sizeof(name), &length, &size, &type, name);
    _attributes.insert( std::pair<std::string, GLuint>(
      name, glGetAttribLocation(_programID, name) ) );
  }
}
    
void ShaderProgram::mapUniforms()
{
  GLint count = 0;
  glGetProgramiv(_programID, GL_ACTIVE_UNIFORMS, &count);

  GLchar name[256];
  GLsizei length = 0;
  GLint size = 0;
  GLenum type;

  for(int i = 0; i < count; i++)
  {
    glGetActiveUniform(_programID, i, sizeof(name), &length, &size, &type, name);
    _uniforms.insert( std::pair<std::string, GLuint>(
      name, glGetUniformLocation(_programID, name) ) );
  }
}


} // namespace Hmi

} // namespace Pt
