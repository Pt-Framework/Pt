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
#include <cassert>

namespace Pt {

namespace Xml {

DtdContext::DtdContext()
: _nodeCount(0)
{}


DtdContext::~DtdContext()
{
    clear();
}


void DtdContext::clear()
{
    resetExpression();

    for(unsigned n = 0; n < _pool.size() ; ++n)
    {
        delete _pool[n];
    }

    _pool.clear();
}


void DtdContext::resetExpression()
{
    //TODO: remove the particles fro pool and delete immediately

    while( ! _fragments.empty() )
        _fragments.pop();
                    
    while( ! _ops.empty() )
        _ops.pop();

    // id of 0 is for Match
    _nodeCount = 1;
}


ContentParticle& DtdContext::finishExpression()
{
    reduceStack();

    if( _fragments.empty() )
    {
        assert(_nodeCount == 1);
        throw std::logic_error("invalid content model");
    }

    if(_fragments.size() > 1)
        throw std::logic_error("DTD syntax error: incomplete expression");

    _fragments.top().patchLeafs( this->getMatch() );

    ContentParticle& particle = _fragments.top().start();

    // TODO reset here, but what about _nodeCount?
    //reset();
    return particle;
}
    
        
void DtdContext::pushOperator(Pt::Char ch)
{
    _ops.push(ch);
}


void DtdContext::pushOpenBrace()
{
    _ops.push('(');
}


void DtdContext::pushClosingBrace()
{
    reduceStack();
}


void DtdContext::pushOperand(ContentParticle& op)
{
    op.setId(_nodeCount++);

    Fragment frag(op);
    frag.setLeaf(op);
    _fragments.push(frag);
}


void DtdContext::reduceStack()
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

            SplitParticle& split = this->getSplit( op2.start() );
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

            SplitParticle& split = this->getSplit( op1.start() );
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

            SplitParticle& split = this->getSplit( op1.start() );
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

            SplitParticle& split = this->getSplit( op1.start() );
            split.setId(_nodeCount++);

            op1.patchLeafs(split);
                    
            Fragment frag( op1.start() );
            frag.setLeaf(split);
            _fragments.push(frag);
            continue;
        }
    }
}


LeafParticle& DtdContext::getLabel(const Pt::String& name)
{
    _pool.reserve(_pool.size() + 1);
    LeafParticle* label = new LeafParticle(name);
    _pool.push_back(label);
    return *label;
}


SplitParticle& DtdContext::getSplit(ContentParticle& to)
{
    _pool.reserve(_pool.size() + 1);
    SplitParticle* split = new SplitParticle(&to);
    _pool.push_back(split);
    return *split;
}


PcDataParticle& DtdContext::getPcData()
{
    _pool.reserve(_pool.size() + 1);
    PcDataParticle* node = new PcDataParticle();
    _pool.push_back(node);
    return *node;
}


MatchParticle& DtdContext::getMatch()
{ 
    return _match; 
}

} // namespace Xml

} // namespace Pt
