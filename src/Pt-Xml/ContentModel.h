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
#ifndef Pt_Xml_ContentModel_h
#define Pt_Xml_ContentModel_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/String.h>

#include <vector>
#include <stack>
#include <cassert>

namespace Pt {

namespace Xml {

class DocTypeDefinition;
class ContentValidator;
class ContentModel;

class ContentParticle
{
    public:
        virtual ~ContentParticle()
        { }

        //! @brief Gets this Particle and follows unlabelled transitions.
        virtual void get(ContentValidator& ctx) const = 0;

        //! @brief Evaluate the XML node and get all following nodes.
        virtual void eval(ContentValidator& ctx, Node& node) const = 0;

        //! @brief Returns true if the node represents a match state.
        virtual bool isValid() const
        { return false; }

        const ContentParticle* out() const
        { return _out; }

        void setNext(ContentParticle& state)
        { _out = &state; }

        void setId(unsigned id)
        { _id = id; }

        unsigned id() const
        { return _id; }

    protected:
        ContentParticle()
        : _out(0)
        , _id(0)
        {}

    private:
        ContentParticle* _out;
        unsigned _id;
};


class ContentValidator
{
    public:
        //!@brief A validator for an undeclared element.
        ContentValidator();

        ContentValidator(const ContentModel& cm);

        bool validate(Node& node);

        bool isComplete() const;

        bool setVisited(unsigned id);

        void addNext(const ContentParticle* p)
        { _current.push_back(p); }

    private:
        const ContentModel* _cm;
        unsigned _stepId;
        std::vector<unsigned> _nodes;
        std::vector<const ContentParticle*> _current;
        std::vector<const ContentParticle*> _next;
};


class SplitParticle : public ContentParticle
{
    public:
        SplitParticle(ContentParticle* to)
        : ContentParticle()
        , _out1(to)
        { }

        virtual void eval(ContentValidator& ctx, Node& node) const;

        virtual void get(ContentValidator& ctx) const ;

    private:
        ContentParticle* _out1;
};


class LeafParticle : public ContentParticle
{
    public:
        LeafParticle(const Pt::String& name)
        : ContentParticle()
        , _name(name)
        { }

        virtual void eval(ContentValidator& ctx, Node& node) const;

        virtual void get(ContentValidator& ctx) const;

    private:
        Pt::String _name;
};


class PcDataParticle : public ContentParticle
{
    public:
        PcDataParticle()
        : ContentParticle()
        { }

        virtual void eval(ContentValidator& ctx, Node& node) const;

        virtual void get(ContentValidator& ctx) const;
};


class MatchParticle : public ContentParticle
{
    public:
        MatchParticle()
        : ContentParticle()
        { setId(0); }

        virtual void eval(ContentValidator& ctx, Node& node) const;
        
        virtual void get(ContentValidator& ctx) const;

        virtual bool isValid() const
        { return true; }
};


class ContentModel
{
    public:
        enum ContentType
        {
            Invalid = 0,
            Expression = 1,
            Empty = 2,
            Any = 3
        };

    public:
        ContentModel()
        : _start(0)
        , _size(0)
        , _type(Invalid)
        {}

        ~ContentModel()
        { }

        bool isEmpty() const
        { return _type == Empty; }

        bool isAny() const
        { return _type == Any; }

        bool isExpression() const
        { return _type == Expression; }

        void setExpression(ContentParticle& start, unsigned n)
        { 
            _start = &start; 
            _size = n;
            _type = Expression;
        }

        void setEmpty()
        { 
            _start = 0;
            _size = 0;
            _type = Empty;
        }

        void setAny()
        { 
            _start = 0;
            _size = 0;
            _type = Any;
        }

        const ContentParticle* first() const
        { return _start; }

        unsigned size() const
        { return _size; }

    private:
        ContentParticle* _start;
        unsigned _size;
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
        ContentModelBuilder(DocTypeDefinition& dtd);

        void clear();

        void setEmpty();

        void setAny();

        void finish(ContentModel& cm, MatchParticle& m);
        
        // TODO: push particles, so we do not have to keep a refrence to a dtd here
        void pushOperator(Pt::Char ch);

        void pushOpenBrace();

        void pushClosingBrace();

        void pushOperand(ContentParticle& op);

    private:
        void reduceStack();

    private:
        DocTypeDefinition* _dtd;
        std::stack<Pt::Char> _ops;
        int _cmtype;
        std::stack<Fragment> _fragments;
        unsigned _nodeCount;
};

} // namespace Xml

} // namespace Pt

#endif
