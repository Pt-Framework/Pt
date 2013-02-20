/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#include "ContentModel.h"
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <cassert>

namespace Pt {

namespace Xml {

ValidationContext::ValidationContext(std::size_t nodeCount)
: _stepId(1)
{
    // all nodes are unvisited
    _nodes.assign(nodeCount, 0);
    _stepId = 1;
}


bool ValidationContext::setVisited(unsigned id)
{ 
    if(_nodes.at(id) == _stepId)
        return true;

    // node not yet visited, mark visited
    _nodes.at(id) = _stepId; 
    return false;
}


void ValidationContext::addNext(const ContentParticle* p)
{ 
    _current.push_back(p); 
}


const std::vector<const ContentParticle*>& ValidationContext::next() const
{ 
    return _current; 
}


void ValidationContext::clear()
{ 
    _current.clear(); 
    _stepId++;
}


void SplitParticle::eval(ValidationContext& ctx, Node& node) const
{ 
}


void SplitParticle::get(ValidationContext& ctx) const
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;
                    
    assert( out() );
    out()->get(ctx);
    
    assert( _out1 );
    _out1->get(ctx);
}


void LeafParticle::eval(ValidationContext& ctx, Node& node) const
{
    StartElement* se = toStartElement(&node);
    if(se && se->name() == _name)
    {
        out()->get(ctx);
    }
}


void LeafParticle::get(ValidationContext& ctx) const
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;

    ctx.addNext(this);
}


void PcDataParticle::eval(ValidationContext& ctx, Node& node) const
{
    Characters* chars = toCharacters(&node);
    if(chars)
        out()->get(ctx);
    else
        out()->eval(ctx, node);
}


void PcDataParticle::get(ValidationContext& ctx) const
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;

    ctx.addNext(this);
}


void MatchParticle::eval(ValidationContext& ctx, Node& node) const
{
}
        

void MatchParticle::get(ValidationContext& ctx) const
{ 
    assert(id() == 0);

    if( ctx.setVisited( this->id() ) )
        return;
    
    ctx.addNext(this); 
}

} // namespace Xml

} // namespace Pt
