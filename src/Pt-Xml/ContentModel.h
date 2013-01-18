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

class ContentModel 
{
    public:
        class Particle
        {
            public:
                virtual ~Particle()
                { }

                //! @brief Gets this Particle and follows unlabelled transitions.
                virtual void get(ContentValidator& ctx) = 0;

                //! @brief Evaluate the XML node and get all following nodes.
                virtual void eval(ContentValidator& ctx, Node& node) = 0;

                //! @brief Returns true if the node represents a match state.
                virtual bool isValid() const
                { return false; }

                Particle* out()
                { return _out; }

                void setNext(Particle& state)
                { _out = &state; }

                void setId(unsigned id)
                { _id = id; }

                unsigned id() const
                { return _id; }

            protected:
                Particle()
                : _out(0)
                , _id(0)
                {}

            private:
                Particle* _out;
                unsigned _id;
        };

        class Split : public Particle
        {
            public:
                Split(Particle* to)
                : Particle()
                , _out1(to)
                { }

                virtual void eval(ContentValidator& ctx, Node& node);

                virtual void get(ContentValidator& ctx) ;

            private:
                Particle* _out1;
        };

        class Leaf : public Particle
        {
            public:
                Leaf(const Pt::String& name)
                : Particle()
                , _name(name)
                { }

                virtual void eval(ContentValidator& ctx, Node& node);

                virtual void get(ContentValidator& ctx);

            private:
                Pt::String _name;
        };

        class PcData : public Particle
        {
            public:
                PcData()
                : Particle()
                { }

                virtual void eval(ContentValidator& ctx, Node& node);

                virtual void get(ContentValidator& ctx);
        };

        class Match : public Particle
        {
            public:
                Match()
                : Particle()
                { setId(0); }

                virtual void eval(ContentValidator& ctx, Node& node);
        
                virtual void get(ContentValidator& ctx);

                virtual bool isValid() const
                { return true; }
        };
    
    public:
        ContentModel()
        : _start(0)
        , _size(0)
        {}

        unsigned size() const
        { return _size; }

        bool isEmpty() const
        { return _start == 0; }

        void setStart(ContentModel::Particle& start, unsigned n)
        { 
            _start = &start; 
            _size = n;
        }

        void setEmpty()
        { 
            _start = 0;
            _size = 0;
        }

        ContentModel::Particle* start()
        { return _start; }

    private:
        ContentModel::Particle* _start;
        unsigned _size;
};


class ContentModelBuilder
{
    private:
        class Fragment
        {
            public:
                explicit Fragment(ContentModel::Particle& start)
                : _start(&start)
                {}

                ContentModel::Particle& start() const
                { return *_start; }

                const std::vector<ContentModel::Particle*>& leafs() const
                { return _leafs; }

                void setLeaf(ContentModel::Particle& next)
                { _leafs.push_back(&next); }

                void setLeafs(const std::vector<ContentModel::Particle*>& leafs)
                { _leafs = leafs; }

                void setLeafs(const std::vector<ContentModel::Particle*>& leafs, const std::vector<ContentModel::Particle*>& leafs2)
                { 
                    _leafs = leafs; 
                    _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
                }

                void setLeafs(const std::vector<ContentModel::Particle*>& leafs, ContentModel::Particle& leaf)
                { 
                    _leafs = leafs; 
                    _leafs.push_back(&leaf);
                }

                void patchLeafs(ContentModel::Particle& to)
                {
                    for(unsigned n = 0; n < _leafs.size(); ++n)
                    {
                        ContentModel::Particle* leaf = _leafs[n];
                        leaf->setNext(to);
                    }
                }

            private:
                ContentModel::Particle* _start;
                std::vector<ContentModel::Particle*> _leafs;
        };

    public:
        ContentModelBuilder(DocTypeDefinition& dtd)
        : _dtd(&dtd)
        , _nodeCount(0)
        {}

        void clear();

        void setEmpty();

        void finish(ContentModel& cm, ContentModel::Match& m);
        
        // TODO: push particles, so we do not have to keep a refrence to a dtd here
        void pushOperator(Pt::Char ch);

        void pushOpenBrace();

        void pushClosingBrace();

        void pushOperand(ContentModel::Particle& op);

    private:
        void reduceStack();

    private:
        DocTypeDefinition* _dtd;
        std::stack<Pt::Char> _ops;
        std::stack<Fragment> _fragments;
        unsigned _nodeCount;
};


class ContentValidator
{
    public:
        //!@brief A validator for an undeclared element.
        ContentValidator()
        : _stepId(1)
        , _cm(0)
        {}

        ContentValidator(ContentModel& cm);

        bool validate(Node& node);

        bool isComplete() const;

        bool setVisited(unsigned id);

        void addNext(ContentModel::Particle* p)
        { _current.push_back(p); }

    private:
        ContentModel* _cm;
        unsigned _stepId;
        std::vector<unsigned> _nodes;
        std::vector<ContentModel::Particle*> _current;
        std::vector<ContentModel::Particle*> _next;
};

} // namespace Xml

} // namespace Pt

#endif
