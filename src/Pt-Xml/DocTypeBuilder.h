/*
 * Copyright (C) 2008-2012 by Marc Boris Duerner
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
#ifndef Pt_Xml_DocTypeBuilder_h
#define Pt_Xml_DocTypeBuilder_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/DocType.h>
#include <Pt/String.h>

#include <stack>
#include <cassert>

namespace Pt {

namespace Xml {

class DocTypeBuilder
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
        DocTypeBuilder(DocType& docType)
        : _docType(&docType)
        , _elemDecl(0)
        {}

        void clear()
        {
            while( ! _fragments.empty() )
                _fragments.pop();
                    
            while( ! _ops.empty() )
                _ops.pop();

            _elemDecl = 0;
        }

        void beginAttList(const Pt::String& name)
        {
            _elemDecl = &_docType->declareElement(name);
        }

        void beginAttribute(AttributeDeclaration* attr)
        {
            assert(_elemDecl);
            _elemDecl->attrListDecl().push(attr);
        }

        void setAttributeMode(AttributeDeclaration::Mode mode)
        {
            assert(_elemDecl);
            _elemDecl->attrListDecl().last().setMode(mode);
        }

        void setAttributeDefault(const Pt::String& value)
        {
            assert(_elemDecl);
            _elemDecl->attrListDecl().last().setDefaultValue(value);
        }

        void beginElement(const Pt::String& name)
        {
            _elemDecl = &_docType->declareElement(name);

            while( ! _fragments.empty() )
                _fragments.pop();
                    
            while( ! _ops.empty() )
                _ops.pop();
        }

        void finish()
        {
            reduceStack();

            if(_fragments.size() != 1 || _elemDecl == 0)
                throw std::logic_error("DTD syntax error: incomplete expression");

            _fragments.top().patchLeafs( _docType->context().getMatch() );
            _elemDecl->setContentModel( _fragments.top().start() );
            _fragments.pop();

            _elemDecl = 0;
        }
        
        void pushOperator(Pt::Char ch)
        {
            _ops.push(ch);
        }

        void pushClosingBrace()
        {
            reduceStack();
        }

        void pushOperand(const Pt::String& name)
        {
            assert(_elemDecl);

            if(name.at(0) == '#')
            {
                if(name != L"#PCDATA")
                    throw std::logic_error("DTD syntax error: expected PCDATA");

                ContentModel::PcData& pcdata = _docType->context().getPcData();
                Fragment frag(pcdata);
                frag.setLeaf(pcdata);
                _fragments.push(frag);
                return;
            }
                
            ContentModel::Label& label = _docType->context().getLabel(name);
            Fragment frag(label);
            frag.setLeaf(label);
            _fragments.push(frag);
        }

        void pushEmpty()
        {
            assert(_elemDecl);

            ContentModel::Empty& e = _docType->context().getEmpty();
            Fragment frag(e);
            frag.setLeaf(e);
            _fragments.push(frag);
        }

    private:
        void reduceStack()
        {
            assert(_elemDecl);

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
                        throw std::logic_error("DTD syntax error: not enough operands for ,");
                    
                    Fragment op2 = _fragments.top();
                    _fragments.pop();

                    Fragment op1 = _fragments.top();
                    _fragments.pop();

                    ContentModel::Split& split = _docType->context().getSplit( op2.start() );
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

                    ContentModel::Split& split = _docType->context().getSplit( op1.start() );
                    
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

                    ContentModel::Split& split = _docType->context().getSplit( op1.start() );

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

                    ContentModel::Split& split = _docType->context().getSplit( op1.start() );

                    op1.patchLeafs(split);
                    
                    Fragment frag( op1.start() );
                    frag.setLeaf(split);
                    _fragments.push(frag);
                    continue;
                }
            }
        }

    private:
        DocType* _docType;
        ElementDeclaration* _elemDecl;
        std::stack<Pt::Char> _ops;
        std::stack<Fragment> _fragments;
};


class DocTypeValidator : private NonCopyable
{
    private:
        class ContentValidator
        {
            public:
                ContentValidator()
                {}

                void setDeclaration(ElementDeclaration& decl)
                {
                    decl.start(_current);
                }

                bool validate(Node& node)
                {
                    for(unsigned n = 0; n < _current.size(); ++n)
                    {
                        ContentModel::Particle* state = _current[n];
                        state->eval(node, _next);
                    }
            
                    _current = _next;
                    _next.clear();

                    return ! _current.empty();
                }

                bool isValid() const
                { 
                    for(unsigned n = 0; n < _current.size(); ++n)
                    {
                        if( _current[n]->isValid() )
                            return true;
                    }
            
                    return false; 
                }

            private:
                std::vector<ContentModel::Particle*> _current;
                std::vector<ContentModel::Particle*> _next;
        };

    public:
        DocTypeValidator(DocType& dtd)
        : _dtd(&dtd)
        {}

        void clear()
        {
            while( ! _decls.empty() )
                _decls.pop();
        }

        bool validate(Node& node)
        {
            bool valid = true;
            
            // TODO: ignorable whitespace

            switch( node.type() )
            {
                case Node::StartElement:
                {
                    StartElement& se = static_cast<StartElement&>(node);

                    if( ! _decls.empty() )
                    {
                        valid = _decls.top().validate(se);
                    }
                    
                    ElementDeclaration* decl = _dtd->findElementDecl( se.name() );
                    ContentValidator validator;
                    if(decl)
                    {
                        validator.setDeclaration(*decl);
                        
                        if( ! decl->attrListDecl().validate( se.attributes() ) )
                            valid = false; 
                    }
                    else
                        valid = false;

                    _decls.push(validator);
                    break;
                }
                
                case Node::Characters:
                {
                    Characters& chars = static_cast<Characters&>(node);

                    if( ! _decls.empty() )
                    {
                        valid = _decls.top().validate(chars);
                    }

                    break;
                }

                case Node::EndElement:
                {
                    valid = _decls.top().isValid();
                    _decls.pop();
                    break;
                }
            }

            return valid;
        }

    private:
        DocType* _dtd;
        std::stack<ContentValidator> _decls;
};

} // namespace Xml

} // namespace Pt

#endif
