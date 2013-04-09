/*
 * Copyright (C) 2013 Marc Boris Duerner
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

#include "ElementModel.h"
#include "AttributeModel.h"
#include "ContentParticle.h"
#include <cassert>

namespace Pt {

namespace Xml {

ElementModel::ElementModel(const QName& name)
: _name(name)
, _start(0)
, _type(Undeclared)
{}
        

ElementModel::~ElementModel()
{
    clear();
}


void ElementModel::clear()
{
    for(unsigned n = 0; n < _particles.size() ; ++n)
    {
        delete _particles[n];
    }

    _particles.clear();

    _type = Undeclared;
}


const QName& ElementModel::qname() const
{
    return _name;
}
        

bool ElementModel::isUndeclared() const
{
    return _type == Undeclared;
}


bool ElementModel::isEmpty() const
{ 
    return _type == Empty; 
}


void ElementModel::setEmpty()
{ 
    clear();
    _start = 0;
    _type = Empty;
}


bool ElementModel::isAny() const
{ 
    return _type == Any; 
}


void ElementModel::setAny()
{ 
    clear();
    _start = 0;
    _type = Any;
}


bool ElementModel::isExpression() const
{ 
    return _type == Expression; 
}


void ElementModel::setExpression(ContentParticle& start)
{ 
    _start = &start; 
    _type = Expression;
}


const ContentParticle* ElementModel::content() const
{ 
    return _start; 
}


std::size_t ElementModel::contentSize() const
{ 
    return _particles.empty() ? 0 : _particles.size() + 1;
}


const AttributeListModel& ElementModel::attributes() const
{ 
    return _attrs; 
}


AttributeListModel& ElementModel::attributes()
{ 
    return _attrs; 
}


LeafParticle& ElementModel::getLabel(const Pt::String& name)
{
    _particles.reserve(_particles.size() + 1);
    LeafParticle* label = new LeafParticle(name);
    _particles.push_back(label);
    label->setId( _particles.size() );
    return *label;
}


SplitParticle& ElementModel::getSplit(ContentParticle& to)
{
    _particles.reserve(_particles.size() + 1);
    SplitParticle* split = new SplitParticle(&to);
    _particles.push_back(split);
    split->setId( _particles.size() );
    return *split;
}


PcDataParticle& ElementModel::getPcData()
{
    _particles.reserve(_particles.size() + 1);
    PcDataParticle* node = new PcDataParticle();
    _particles.push_back(node);
    node->setId( _particles.size() );
    return *node;
}

// TODO
static MatchParticle match;

MatchParticle& ElementModel::getMatch()
{ 
    return match; 
}

} // namespace Xml

} // namespace Pt
