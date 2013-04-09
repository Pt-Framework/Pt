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

#include "ContentModel.h"
#include "ContentParticle.h"
#include <cassert>

namespace Pt {

namespace Xml {

ContentModel::ContentModel()
: _start(0)
, _type(Undeclared)
{}
        

ContentModel::~ContentModel()
{
    clear();
}


void ContentModel::clear()
{
    for(unsigned n = 0; n < _particles.size() ; ++n)
    {
        delete _particles[n];
    }

    _particles.clear();

    _type = Undeclared;
}


bool ContentModel::isUndeclared() const
{
    return _type == Undeclared;
}


bool ContentModel::isEmpty() const
{ 
    return _type == Empty; 
}


void ContentModel::setEmpty()
{ 
    clear();
    _start = 0;
    _type = Empty;
}


bool ContentModel::isAny() const
{ 
    return _type == Any; 
}


void ContentModel::setAny()
{ 
    clear();
    _start = 0;
    _type = Any;
}


bool ContentModel::isExpression() const
{ 
    return _type == Expression; 
}


void ContentModel::setExpression(ContentParticle& start)
{ 
    _start = &start; 
    _type = Expression;
}


const ContentParticle* ContentModel::expression() const
{ 
    return _start; 
}


std::size_t ContentModel::size() const
{ 
    return _particles.empty() ? 0 : _particles.size() + 1;
}


LeafParticle& ContentModel::getLabel(const Pt::String& name)
{
    _particles.reserve(_particles.size() + 1);
    LeafParticle* label = new LeafParticle(name);
    _particles.push_back(label);
    label->setId( _particles.size() );
    return *label;
}


SplitParticle& ContentModel::getSplit(ContentParticle& to)
{
    _particles.reserve(_particles.size() + 1);
    SplitParticle* split = new SplitParticle(&to);
    _particles.push_back(split);
    split->setId( _particles.size() );
    return *split;
}


PcDataParticle& ContentModel::getPcData()
{
    _particles.reserve(_particles.size() + 1);
    PcDataParticle* node = new PcDataParticle();
    _particles.push_back(node);
    node->setId( _particles.size() );
    return *node;
}

// TODO
static MatchParticle match;

MatchParticle& ContentModel::getMatch()
{ 
    return match; 
}




void ContentModelBuilder::Fragment::setLeafs(const std::vector<ContentParticle*>& leafs, 
                                             const std::vector<ContentParticle*>& leafs2)
{ 
    _leafs = leafs; 
    _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
}


void ContentModelBuilder::Fragment::setLeafs(const std::vector<ContentParticle*>& leafs, 
                                             ContentParticle& leaf)
{ 
    _leafs = leafs; 
    _leafs.push_back(&leaf);
}


void ContentModelBuilder::Fragment::patchLeafs(ContentParticle& to)
{
    for(unsigned n = 0; n < _leafs.size(); ++n)
    {
        ContentParticle* leaf = _leafs[n];
        leaf->setNext(to);
    }
}


ContentModelBuilder::ContentModelBuilder()
: _cm(0)
{}


ContentModelBuilder::~ContentModelBuilder()
{
    reset();
}


void ContentModelBuilder::reset(ContentModel& cm)
{
    reset();
    _cm = &cm;
}


void ContentModelBuilder::reset()
{
    while( ! _fragments.empty() )
        _fragments.pop();
                    
    while( ! _ops.empty() )
        _ops.pop();

    _cm = 0;
}


bool ContentModelBuilder::finish()
{
    if( ! _cm)
        return false;

    if( ! reduceStack() )
        return false;

    if(_fragments.size() != 1)
        return false;

    _fragments.top().patchLeafs( _cm->getMatch() );

    ContentParticle& particle = _fragments.top().start();
    _cm->setExpression(particle);

    reset();
    return true;
}
    
        
void ContentModelBuilder::pushOperator(Pt::Char ch)
{
    if( ! _cm)
        return;

    _ops.push(ch);
}


void ContentModelBuilder::pushScope()
{
    if( ! _cm)
        return;

    _ops.push('(');
}


bool ContentModelBuilder::reduceScope()
{
    if( ! _cm)
        return false;

    return reduceStack();
}


void ContentModelBuilder::pushOperand(const String& name)
{
    if( ! _cm)
        return;

    if(name== L"#PCDATA")
    {
        PcDataParticle& pcdata = _cm->getPcData();
        pushOperand(pcdata);
                
        // TODO: allow #PCDATA only at beginning of first '('
        // TODO: allow only '|' as next token
        return;
    }
                
    LeafParticle& leaf = _cm->getLabel(name);
    pushOperand(leaf);
}


void ContentModelBuilder::pushOperand(ContentParticle& op)
{
    Fragment frag(op);
    frag.setLeaf(op);
    _fragments.push(frag);
}


bool ContentModelBuilder::reduceStack()
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
                return false;
                    
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
                return false;
                    
            Fragment op2 = _fragments.top();
            _fragments.pop();

            Fragment op1 = _fragments.top();
            _fragments.pop();

            SplitParticle& split = _cm->getSplit( op2.start() );
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
                return false;
                    
            Fragment op1 = _fragments.top();
            _fragments.pop();

            SplitParticle& split = _cm->getSplit( op1.start() );
                    
            Fragment frag(split);
            frag.setLeafs(op1.leafs(), split);
            _fragments.push(frag);
            continue;
        }

        if(_ops.top() == '*')
        {
            _ops.pop();
                 
            if( _fragments.empty() )
                return false;
                    
            Fragment op1 = _fragments.top();
            _fragments.pop();

            SplitParticle& split = _cm->getSplit( op1.start() );

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
                return false;
                    
            Fragment op1 = _fragments.top();
            _fragments.pop();

            SplitParticle& split = _cm->getSplit( op1.start() );

            op1.patchLeafs(split);
                    
            Fragment frag( op1.start() );
            frag.setLeaf(split);
            _fragments.push(frag);
            continue;
        }
    }

    return true;
}

} // namespace Xml

} // namespace Pt
