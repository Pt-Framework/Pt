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
#include "DocTypeDefinition.h"

namespace Pt {

namespace Xml {

void ContentModel::Split::eval(ContentValidator& ctx, Node& node)
{ 
}


void ContentModel::Split::get(ContentValidator& ctx) 
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;
                    
    assert( out() );
    out()->get(ctx);
    
    assert( _out1 );
    _out1->get(ctx);
}


void ContentModel::Leaf::eval(ContentValidator& ctx, Node& node)
{
    StartElement* se = toStartElement(&node);
    if(se && se->name() == _name)
    {
        out()->get(ctx);
    }
}


void ContentModel::Leaf::get(ContentValidator& ctx) 
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;

    ctx.addNext(this);
}


void ContentModel::PcData::eval(ContentValidator& ctx, Node& node)
{
    Characters* chars = toCharacters(&node);
    if(chars)
        out()->get(ctx);
    else
        out()->eval(ctx, node);
}


void ContentModel::PcData::get(ContentValidator& ctx) 
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;

    ctx.addNext(this);
}


void ContentModel::Match::eval(ContentValidator& ctx, Node& node)
{
}
        

void ContentModel::Match::get(ContentValidator& ctx) 
{ 
    assert(id() == 0);

    if( ctx.setVisited( this->id() ) )
        return;
    
    ctx.addNext(this); 
}


void ContentModelBuilder::clear()
{
    while( ! _fragments.empty() )
        _fragments.pop();
                    
    while( ! _ops.empty() )
        _ops.pop();

    // id of 0 is for Match
    _nodeCount = 1;
}


void ContentModelBuilder::setEmpty()
{
    clear();
}


void ContentModelBuilder::finish(ContentModel& cm, ContentModel::Match& match)
{
    reduceStack();

    if( _fragments.empty() )
    {
        assert(_nodeCount == 1);
        cm.setEmpty();
        return;
    }

    if(_fragments.size() > 1)
        throw std::logic_error("DTD syntax error: incomplete expression");

    _fragments.top().patchLeafs(match);
    ContentModel::Particle& particle = _fragments.top().start();
    cm.setStart(particle, _nodeCount);

    clear();
}
    
        
void ContentModelBuilder::pushOperator(Pt::Char ch)
{
    _ops.push(ch);
}


void ContentModelBuilder::pushOpenBrace()
{
    _ops.push('(');
}


void ContentModelBuilder::pushClosingBrace()
{
    reduceStack();
}


void ContentModelBuilder::pushOperand(ContentModel::Particle& op)
{
    op.setId(_nodeCount++);

    Fragment frag(op);
    frag.setLeaf(op);
    _fragments.push(frag);
}


void ContentModelBuilder::reduceStack()
{
    for(;;)
    {
        if( _ops.empty() )
            break;

        if(_ops.top() == '(')
        {
            _ops.pop();
            break;
        }

        if(_ops.top() == ',')
        {
            _ops.pop();
                 
            if( _fragments.size() < 2 )
                throw std::logic_error("DTD syntax error: not enough operands for ,");
                    
            Fragment op2 = _fragments.top();
            _fragments.pop();

            Fragment op1 = _fragments.top();
            _fragments.pop();

            op1.patchLeafs( op2.start() );
                    
            Fragment frag( op1.start() );
            frag.setLeafs( op2.leafs() );
            _fragments.push(frag);
            continue;
        }

        if(_ops.top() == '|')
        {
            _ops.pop();
                 
            if( _fragments.size() < 2 )
                throw std::logic_error("DTD syntax error: not enough operands for |");
                    
            Fragment op2 = _fragments.top();
            _fragments.pop();

            Fragment op1 = _fragments.top();
            _fragments.pop();

            ContentModel::Split& split = _dtd->getSplit( op2.start() );
            split.setId(_nodeCount++);
            split.setNext( op1.start() );

            Fragment frag(split);
            frag.setLeafs( op1.leafs(), op2.leafs() );
            _fragments.push(frag);
            continue;
        }

        if(_ops.top() == '?')
        {
            _ops.pop();
                 
            if( _fragments.empty() )
                throw std::logic_error("DTD syntax error: not enough operands for ?");
                    
            Fragment op1 = _fragments.top();
            _fragments.pop();

            ContentModel::Split& split = _dtd->getSplit( op1.start() );
            split.setId(_nodeCount++);
                    
            Fragment frag(split);
            frag.setLeafs(op1.leafs(), split);
            _fragments.push(frag);
            continue;
        }

        if(_ops.top() == '*')
        {
            _ops.pop();
                 
            if( _fragments.empty() )
                throw std::logic_error("DTD syntax error: not enough operands for *");
                    
            Fragment op1 = _fragments.top();
            _fragments.pop();

            ContentModel::Split& split = _dtd->getSplit( op1.start() );
            split.setId(_nodeCount++);

            op1.patchLeafs(split);
                    
            Fragment frag( split );
            frag.setLeaf(split);
            _fragments.push(frag);
            continue;
        }

        if(_ops.top() == '+')
        {
            _ops.pop();
                 
            if( _fragments.empty() )
                throw std::logic_error("DTD syntax error: not enough operands for +");
                    
            Fragment op1 = _fragments.top();
            _fragments.pop();

            ContentModel::Split& split = _dtd->getSplit( op1.start() );
            split.setId(_nodeCount++);

            op1.patchLeafs(split);
                    
            Fragment frag( op1.start() );
            frag.setLeaf(split);
            _fragments.push(frag);
            continue;
        }
    }
}


ContentValidator::ContentValidator(ContentModel& cm)
: _cm(&cm)
, _stepId(1)
{
    // all nodes are unvisited
    _nodes.assign(cm.size(), 0);
    _stepId = 1;

    if( cm.start() )
        cm.start()->get(*this);
}


bool ContentValidator::validate(Node& node)
{
    // handle ignorable WS and EMPTY separately, so indentation in XML
    // documents does not lead to costly state transitions. 
    if( Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&node) )
    {
        if( chars->isIgnorable() )
        {
            // if _cm is null, the element was undeclared and WS should
            // not lead to a validation error
            if( ! _cm )
                return true;

            // special rule for EMPTY, not even WS is allowed
            if( _cm->isEmpty() )
                return false;

            return true;
        }
    }

    _stepId++;
    _next = _current;
    _current.clear();

    for(unsigned n = 0; n < _next.size(); ++n)
    {
        _next[n]->eval(*this, node);
    }

    // no follow up particles means validation error
    return ! _current.empty();
}


bool ContentValidator::isComplete() const
{ 
    // if _cm is null, the element was undeclared
    if( ! _cm || _cm->isEmpty() )
        return true;
            
    // at the end of the validation, at least one current particle
    // must be a match particle, otherwise there was more content
    // expected to come
    for(unsigned n = 0; n < _current.size(); ++n)
    {
        if( _current[n]->isValid() )
            return true;
    }
            
    return false; 
}


bool ContentValidator::setVisited(unsigned id)
{ 
    if(_nodes.at(id) == _stepId)
        return true;

    // node not yet visited, mark visited
    _nodes.at(id) = _stepId; 
    return false;
}

} // namespace Xml

} // namespace Pt
