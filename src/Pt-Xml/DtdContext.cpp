/*
 * Copyright (C) 2013 by Marc Boris Duerner
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
#include "DtdContext.h"
#include "ElementDeclaration.h"
#include "Pt/Xml/DocTypeDefinition.h"
#include <cassert>

namespace Pt {

namespace Xml {

ContentModelBuilder::ContentModelBuilder()
: _elem(0)
{}


ContentModelBuilder::~ContentModelBuilder()
{
    reset();
}


void ContentModelBuilder::reset(ElementModel& elem)
{
    reset();
    _elem = &elem;
}


void ContentModelBuilder::reset()
{
    //TODO: remove the particles fro pool and delete immediately

    while( ! _fragments.empty() )
        _fragments.pop();
                    
    while( ! _ops.empty() )
        _ops.pop();

    _elem = 0;
}


void ContentModelBuilder::finish()
{
    if( ! _elem)
        return;

    reduceStack();

    if( _fragments.empty() )
    {
        throw std::logic_error("invalid content model");
    }

    if(_fragments.size() > 1)
        throw std::logic_error("DTD syntax error: incomplete expression");

    _fragments.top().patchLeafs( _elem->getMatch() );

    ContentParticle& particle = _fragments.top().start();

    if(_elem) // skip duplicates
        _elem->setExpression(particle);

    reset();
}
    
        
void ContentModelBuilder::pushOperator(Pt::Char ch)
{
    if( ! _elem)
        return;

    _ops.push(ch);
}


void ContentModelBuilder::pushOpenBrace()
{
    if( ! _elem)
        return;

    _ops.push('(');
}


void ContentModelBuilder::pushClosingBrace()
{
    if( ! _elem)
        return;

    reduceStack();
}


void ContentModelBuilder::pushDtdOperand(const String& name)
{
    if( ! _elem)
        return;

    if(name.at(0) == '#')
    {
        if(name != L"#PCDATA")
            throw std::logic_error("DTD syntax error: expected PCDATA");

        PcDataParticle& pcdata = _elem->getPcData();
        pushOperand(pcdata);
                
        // TODO: allow #PCDATA only at beginning of first '('
        // TODO: allow only '|' as next token
        return;
    }
                
    LeafParticle& leaf = _elem->getLabel(name);
    pushOperand(leaf);
}


void ContentModelBuilder::pushOperand(ContentParticle& op)
{
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

            SplitParticle& split = _elem->getSplit( op2.start() );
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

            SplitParticle& split = _elem->getSplit( op1.start() );
                    
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

            SplitParticle& split = _elem->getSplit( op1.start() );

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

            SplitParticle& split = _elem->getSplit( op1.start() );

            op1.patchLeafs(split);
                    
            Fragment frag( op1.start() );
            frag.setLeaf(split);
            _fragments.push(frag);
            continue;
        }
    }
}

} // namespace Xml

} // namespace Pt
