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

void SplitParticle::eval(ContentValidator& ctx, Node& node)
{ 
}


void SplitParticle::get(ContentValidator& ctx) 
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;
                    
    assert( out() );
    out()->get(ctx);
    
    assert( _out1 );
    _out1->get(ctx);
}


void LeafParticle::eval(ContentValidator& ctx, Node& node)
{
    StartElement* se = toStartElement(&node);
    if(se && se->name() == _name)
    {
        out()->get(ctx);
    }
}


void LeafParticle::get(ContentValidator& ctx) 
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;

    ctx.addNext(this);
}


void PcDataParticle::eval(ContentValidator& ctx, Node& node)
{
    Characters* chars = toCharacters(&node);
    if(chars)
        out()->get(ctx);
    else
        out()->eval(ctx, node);
}


void PcDataParticle::get(ContentValidator& ctx) 
{
    assert(id() != 0);

    if( ctx.setVisited( this->id() ) )
        return;

    ctx.addNext(this);
}


void MatchParticle::eval(ContentValidator& ctx, Node& node)
{
}
        

void MatchParticle::get(ContentValidator& ctx) 
{ 
    assert(id() == 0);

    if( ctx.setVisited( this->id() ) )
        return;
    
    ctx.addNext(this); 
}


ContentModelBuilder::ContentModelBuilder(DocTypeDefinition& dtd)
: _dtd(&dtd)
, _cmtype(ElementDeclaration::Expression)
, _nodeCount(0)
{}


void ContentModelBuilder::clear()
{
    while( ! _fragments.empty() )
        _fragments.pop();
                    
    while( ! _ops.empty() )
        _ops.pop();

    // id of 0 is for Match
    _nodeCount = 1;

    _cmtype = ElementDeclaration::Expression;
}


void ContentModelBuilder::setEmpty()
{
    clear();
    _cmtype = ElementDeclaration::Empty;
}


void ContentModelBuilder::setAny()
{
    clear();
    _cmtype = ElementDeclaration::Any;
}


void ContentModelBuilder::finish(ElementDeclaration& elem, MatchParticle& match)
{
    reduceStack();

    if( _fragments.empty() )
    {
        assert(_nodeCount == 1);

        if(_cmtype == ElementDeclaration::Any)
            elem.setAny();
        else if(_cmtype == ElementDeclaration::Empty)
            elem.setEmpty();
        else
            throw std::logic_error("invalid content model");
        
        return;
    }

    if(_fragments.size() > 1)
        throw std::logic_error("DTD syntax error: incomplete expression");

    _fragments.top().patchLeafs(match);
    ContentParticle& particle = _fragments.top().start();
    elem.setExpression(particle, _nodeCount);

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


void ContentModelBuilder::pushOperand(ContentParticle& op)
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

            SplitParticle& split = _dtd->getSplit( op2.start() );
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

            SplitParticle& split = _dtd->getSplit( op1.start() );
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

            SplitParticle& split = _dtd->getSplit( op1.start() );
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

            SplitParticle& split = _dtd->getSplit( op1.start() );
            split.setId(_nodeCount++);

            op1.patchLeafs(split);
                    
            Fragment frag( op1.start() );
            frag.setLeaf(split);
            _fragments.push(frag);
            continue;
        }
    }
}


ContentValidator::ContentValidator(ElementDeclaration& elemDecl)
: _elemDecl(&elemDecl)
, _stepId(1)
{
    // all nodes are unvisited
    _nodes.assign(elemDecl.size(), 0);
    _stepId = 1;

    if( elemDecl.start() )
        elemDecl.start()->get(*this);
}


bool ContentValidator::validate(Node& node)
{
    if( _elemDecl->isAny() )
        return true;

    // handle ignorable WS and EMPTY separately, so indentation in XML
    // documents does not lead to costly state transitions. 
    if( Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&node) )
    {
        if( chars->isIgnorable() )
        {
            // if _cm is null, the element was undeclared and WS should
            // not lead to a validation error
            if( ! _elemDecl )
                return true;

            // special rule for EMPTY, not even WS is allowed
            if( _elemDecl->isEmpty() )
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
    if( ! _elemDecl || _elemDecl->isEmpty() || _elemDecl->isAny() )
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
