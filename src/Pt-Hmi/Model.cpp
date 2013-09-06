/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Controller.h>

namespace Pt{
namespace Hmi{

Model::Model()
: _properties()
, DefineProperty(Enable,true)
, DefineProperty(Tag,0)
, _ctrl(0)
{ 

}

Model::~Model()
{ 

}

void Model::registerProperty(PropertyBase* prop)
{
    std::pair<std::string,PropertyBase*> pair(prop->name(), prop);
    _properties.insert(pair);
}
    
const Controller* Model::controller() const
{
	return _ctrl;
}

Controller* Model::controller()
{
	return _ctrl;
}

void Model::setController( Controller* ctrl)
{
	_ctrl = ctrl;
}


}}
