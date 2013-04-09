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

#ifndef Pt_Xml_ContentModel_h
#define Pt_Xml_ContentModel_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/QName.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <stack>
#include <cstddef>

namespace Pt {

namespace Xml {

class ContentParticle;
class LeafParticle;
class SplitParticle;
class PcDataParticle;
class MatchParticle;

class ContentModel : private NonCopyable
{
    enum ContentType
    {
        Undeclared = 0,
        Expression = 1,
        Empty = 2,
        Any = 3
    };

    public:
        ContentModel();
        
        ~ContentModel();

        void clear();

        bool isUndeclared() const;
        
        bool isEmpty() const;

        void setEmpty();

        bool isAny() const;

        void setAny();

        bool isExpression() const;

        void setExpression(ContentParticle& start);

        const ContentParticle* expression() const;

        std::size_t size() const;

    public:
        //! @internal use allocator
        LeafParticle& getLabel(const Pt::String& name);

        //! @internal use allocator
        SplitParticle& getSplit(ContentParticle& to);

        //! @internal use allocator
        PcDataParticle& getPcData();

        //! @internal use allocator
        MatchParticle& getMatch();

    private:
        ContentParticle* _start;
        std::vector<ContentParticle*> _particles;
        ContentType _type;
};


class ContentModelBuilder
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

                void setLeafs(const std::vector<ContentParticle*>& leafs, const std::vector<ContentParticle*>& leafs2);

                void setLeafs(const std::vector<ContentParticle*>& leafs, ContentParticle& leaf);

                void patchLeafs(ContentParticle& to);

            private:
                ContentParticle* _start;
                std::vector<ContentParticle*> _leafs;
        };

    public:
        ContentModelBuilder();

        ~ContentModelBuilder();

        void reset(ContentModel& cm);

        void reset();

        bool finish();
        
        void pushOperator(Pt::Char ch);

        void pushScope();

        bool reduceScope();

        void pushOperand(const String& name);

    private:
        void pushOperand(ContentParticle& op);

        bool reduceStack();

    private:
        ContentModel* _cm;
        std::stack<Pt::Char> _ops;
        std::stack<Fragment> _fragments;
};

} // namespace Xml

} // namespace Pt

#endif
