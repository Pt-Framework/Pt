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
#ifndef Pt_Xml_DtdContext_h
#define Pt_Xml_DtdContext_h

#include "ContentModel.h"
#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <stack>

namespace Pt {

namespace Xml {

class DtdContext
{
    private:
        class Fragment
        {
            public:
                explicit Fragment(ContentParticle& start)
                : _start(&start)
                {}

                ContentParticle& start() const
                { return *_start; }

                const std::vector<ContentParticle*>& leafs() const
                { return _leafs; }

                void setLeaf(ContentParticle& next)
                { _leafs.push_back(&next); }

                void setLeafs(const std::vector<ContentParticle*>& leafs)
                { _leafs = leafs; }

                void setLeafs(const std::vector<ContentParticle*>& leafs, const std::vector<ContentParticle*>& leafs2)
                { 
                    _leafs = leafs; 
                    _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
                }

                void setLeafs(const std::vector<ContentParticle*>& leafs, ContentParticle& leaf)
                { 
                    _leafs = leafs; 
                    _leafs.push_back(&leaf);
                }

                void patchLeafs(ContentParticle& to)
                {
                    for(unsigned n = 0; n < _leafs.size(); ++n)
                    {
                        ContentParticle* leaf = _leafs[n];
                        leaf->setNext(to);
                    }
                }

            private:
                ContentParticle* _start;
                std::vector<ContentParticle*> _leafs;
        };

    public:
        DtdContext();

        ~DtdContext();

        void clear();

        void resetExpression();

        ContentParticle& finishExpression();

        std::size_t expressionSize() const
        { return _nodeCount; }
        
        void pushOperator(Pt::Char ch);

        void pushOpenBrace();

        void pushClosingBrace();

        void pushOperand(ContentParticle& op);

        LeafParticle& getLabel(const Pt::String& name);

        SplitParticle& getSplit(ContentParticle& to);

        PcDataParticle& getPcData();

        MatchParticle& getMatch();

    private:
        void reduceStack();

    private:
        std::vector<ContentParticle*> _pool;
        unsigned _nodeCount;
        MatchParticle _match;
        std::stack<Pt::Char> _ops;
        std::stack<Fragment> _fragments;
};

} // namespace Xml

} // namespace Pt

#endif
