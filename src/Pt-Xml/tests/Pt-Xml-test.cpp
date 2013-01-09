/*
 * Copyright (C) 2005-2006 by Marc Boris Dürner
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
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>

#include <vector>
#include <stack>
#include <map>
#include <cassert>

class DtdAttrDecl
{
    public:
        enum Mode
        {
            Required = 0,
            Implied = 1,
            Fixed = 2,
            Default = 3
        };

    public:
        DtdAttrDecl()
        : _mode(Default)
        {}

        virtual ~DtdAttrDecl()
        { }

        void setMode(Mode mode)
        { _mode = mode; }

        Mode mode() const
        { return _mode; }

        const Pt::String& name() const
        { return _name; }

        void setName(const Pt::String& name)
        { _name = name; }

        void setDefault(const Pt::String& def)
        { _default = def; }

        virtual bool match(const Pt::String& value) const = 0;
       
        bool postMatch(Pt::Xml::AttributeList& list) const
        {
            switch(_mode)
            {
                case Required:
                    return false;

                case Implied:
                    return true;

                case Fixed:
                case Default:
                    break;
            };

            Pt::Xml::Attribute attr;
            attr.setName(_name);
            attr.setValue(_default);
            list.add(attr);
            return true;
        }

    private:
        Mode _mode;
        Pt::String _name;
        Pt::String _default;
};


class DtdAttrCDataDecl : public DtdAttrDecl
{
    public:
        DtdAttrCDataDecl()
        : DtdAttrDecl()
        {}

        virtual bool match(const Pt::String& value) const
        { return true; }
};


class DtdAttrListDecl
{
    typedef std::vector<DtdAttrDecl*> AttrDecls;

    public:
        DtdAttrListDecl()
        {}

        ~DtdAttrListDecl()
        {
            AttrDecls::iterator it;
            for(it = _attrs.begin(); it != _attrs.end(); ++it)
            {
                delete *it;
            }
        }

        const std::vector<DtdAttrDecl*>& decls() const
        {
            return _attrs;
        }

        void push(DtdAttrDecl* decl)
        { _attrs.push_back(decl); }

        bool validate(Pt::Xml::AttributeList& attrs) const
        {
            std::vector<DtdAttrDecl*>  attrDecls = _attrs;
            //
            // match attributes against declarations, remove declartions
            // that match an attribute
            //
            Pt::Xml::AttributeList::ConstIterator attr;
            for(attr = attrs.begin(); attr != attrs.end(); ++attr)
            {
                AttrDecls::iterator it;
                 
                for(it = attrDecls.begin(); it != attrDecls.end(); ++it)
                {
                    if( (*it)->name() == attr->name() )
                    {
                        break;
                    }
                }

                if( it == attrDecls.end() )
                    return false;

                if( ! (*it)->match( attr->value() ) )
                    return false;

                attrDecls.erase(it);
            }

            //
            // post process unmatched declarations e.g. get default values
            // and check for missing required attributes
            //
            AttrDecls::iterator decl;
            for(decl = attrDecls.begin(); decl != attrDecls.end(); ++decl)
            {
                if( ! (*decl)->postMatch(attrs) )
                    return false;
            }

            return true;
        }

    private:
        AttrDecls _attrs;
};


class DtdElementDecl
{
    public:
        class Node
        {
            public:
                virtual ~Node()
                { }

                //! @brief Gets this node and follows unlabelled transitions.
                virtual void get(std::vector<Node*>& nodes) = 0;

                virtual void eval(Pt::Xml::Node& node, std::vector<DtdElementDecl::Node*>& states) = 0;

                virtual bool isValid() const
                { return false; }

                Node* out()
                { return _out; }

                void setNext(Node& state)
                { _out = &state; }

            protected:
                Node()
                : _out(0)
                {}

            private:
                Node* _out;
        };

        class Split : public Node
        {
            public:
                Split(Node* to)
                : Node()
                , _out1(to)
                { }

                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& nodes)
                { }

                virtual void get(std::vector<Node*>& nodes) 
                {
                    assert( out() );
                    out()->get(nodes);
                    _out1->get(nodes);
                }

            private:
                Node* _out1;
        };

        class Label : public Node
        {
            public:
                Label(const Pt::String& name)
                : Node()
                , _name(name)
                { }

                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& states)
                {
                    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&node);
                    if(se && se->name() == _name)
                        out()->get(states);
                }

                virtual void get(std::vector<Node*>& states) 
                {
                    states.push_back(this);
                }

            private:
                Pt::String _name;
        };

        class PcData : public Node
        {
            public:
                PcData()
                : Node()
                { }

                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& states)
                {
                    Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&node);
                    if(chars)
                        out()->get(states);
                }

                virtual void get(std::vector<Node*>& states) 
                {
                    states.push_back(this);
                }
        };

        class Empty : public Node
        {
            public:
                Empty()
                : Node()
                { }

                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& states)
                { }
        
                virtual void get(std::vector<Node*>& states) 
                { states.push_back(this); }

                virtual bool isValid() const
                { return true; }
        };

        class Match : public Node
        {
            public:
                Match()
                : Node()
                { }

                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& states)
                { }
        
                virtual void get(std::vector<Node*>& states) 
                { states.push_back(this); }

                virtual bool isValid() const
                { return true; }
        };

    public:
        DtdElementDecl()
        : _start(0)
        , _attr(0)
        {}

        Node* startNode()
        { return _start; }

        void setContentExpr(Node& s)
        { _start = &s; }

        void setAttrListDecl(DtdAttrListDecl& attr)
        { _attr = &attr; }

        bool validate(Pt::Xml::AttributeList& attrs)
        {
            return _attr ? _attr->validate(attrs) : attrs.empty();
        }

    private:
        Node* _start;
        DtdAttrListDecl* _attr;
};


class DocTypeDefinition
{
    public:
        DocTypeDefinition()
        {}

        ~DocTypeDefinition()
        {
            for(unsigned n = 0; n < _pool.size() ; ++n)
            {
                delete _pool[n];
            }
        }

        DtdElementDecl::Label& createLabel(const Pt::String& name)
        {
            _pool.reserve(_pool.size() + 1);
            DtdElementDecl::Label* label = new DtdElementDecl::Label(name);
            _pool.push_back(label);
            return *label;
        }

        DtdElementDecl::Split& createSplit(DtdElementDecl::Node& to)
        {
            _pool.reserve(_pool.size() + 1);
            DtdElementDecl::Split* split = new DtdElementDecl::Split(&to);
            _pool.push_back(split);
            return *split;
        }

        DtdElementDecl::PcData& createPcData()
        {
            _pool.reserve(_pool.size() + 1);
            DtdElementDecl::PcData* node = new DtdElementDecl::PcData();
            _pool.push_back(node);
            return *node;
        }

        DtdElementDecl::Empty& createEmpty()
        { return _dtdEmpty; }

        DtdElementDecl::Node& createEnd()
        { return _dtdEnd; }

        DtdElementDecl& addElementDecl(const Pt::String& name)
        { 
            return _elemDecls[name]; 
        }

        DtdElementDecl* findElementDecl(const Pt::String& name)
        {
            DtdElementDecl* decl = 0;

            std::map<Pt::String, DtdElementDecl>::iterator it;
            it = _elemDecls.find(name);

            if(it != _elemDecls.end())
                decl = &it->second;

            return decl; 
        }

    private:
        // TODO: move to DtdContext
        DtdElementDecl::Empty _dtdEmpty;
        DtdElementDecl::Match _dtdEnd;
        std::vector<DtdElementDecl::Node*> _pool;
        ///////////

        std::map<Pt::String, DtdElementDecl> _elemDecls;
};


class DtdElementDeclBuilder
{
    public:
        class Fragment
        {
            public:
                explicit Fragment(DtdElementDecl::Node& start)
                : _start(&start)
                {}

                DtdElementDecl::Node& start() const
                { return *_start; }

                const std::vector<DtdElementDecl::Node*>& leafs() const
                { return _leafs; }

                void setLeaf(DtdElementDecl::Node& next)
                { _leafs.push_back(&next); }

                void setLeafs(const std::vector<DtdElementDecl::Node*>& leafs)
                { _leafs = leafs; }

                void setLeafs(const std::vector<DtdElementDecl::Node*>& leafs, const std::vector<DtdElementDecl::Node*>& leafs2)
                { 
                    _leafs = leafs; 
                    _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
                }

                void setLeafs(const std::vector<DtdElementDecl::Node*>& leafs, DtdElementDecl::Node& leaf)
                { 
                    _leafs = leafs; 
                    _leafs.push_back(&leaf);
                }

                void patchLeafs(DtdElementDecl::Node& to)
                {
                    for(unsigned n = 0; n < _leafs.size(); ++n)
                    {
                        DtdElementDecl::Node* leaf = _leafs[n];
                        leaf->setNext(to);
                    }
                }

            private:
                DtdElementDecl::Node* _start;
                std::vector<DtdElementDecl::Node*> _leafs;
        };

    public:
        DtdElementDeclBuilder(DocTypeDefinition& dtd)
        : _dtd(&dtd)
        {}

        void clear()
        {
            while( ! _fragments.empty() )
                _fragments.pop();

            while( ! _ops.empty() )
                _ops.pop();
        }

        DtdElementDecl::Node& finish()
        {
            reduceStack();

            if(_fragments.size() != 1)
                throw std::logic_error("DTD syntax error: incomplete expression");

            _fragments.top().patchLeafs( _dtd->createEnd() );
            DtdElementDecl::Node& start = _fragments.top().start();
            _fragments.pop();
            return start;
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
            if(name.at(0) == '#')
            {
                if(name != L"#PCDATA")
                    throw std::logic_error("DTD syntax error: expected PCDATA");

                DtdElementDecl::PcData& pcdata = _dtd->createPcData();
                Fragment frag(pcdata);
                frag.setLeaf(pcdata);
                _fragments.push(frag);
                return;
            }
                
            DtdElementDecl::Label& label =_dtd->createLabel(name);
            Fragment frag(label);
            frag.setLeaf(label);
            _fragments.push(frag);
        }

        void pushEmpty()
        {
            DtdElementDecl::Empty& e = _dtd->createEmpty();
            Fragment frag(e);
            frag.setLeaf(e);
            _fragments.push(frag);
        }

    private:
        void reduceStack()
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
                        throw std::logic_error("DTD syntax error: not enough operands for ,");
                    
                    Fragment op2 = _fragments.top();
                    _fragments.pop();

                    Fragment op1 = _fragments.top();
                    _fragments.pop();

                    DtdElementDecl::Split& split = _dtd->createSplit( op2.start() );
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

                    DtdElementDecl::Split& split = _dtd->createSplit( op1.start() );
                    
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

                    DtdElementDecl::Split& split = _dtd->createSplit( op1.start() );

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

                    DtdElementDecl::Split& split = _dtd->createSplit( op1.start() );

                    op1.patchLeafs(split);
                    
                    Fragment frag( op1.start() );
                    frag.setLeaf(split);
                    _fragments.push(frag);
                    continue;
                }
            }
        }

    private:
        DocTypeDefinition* _dtd;
        std::stack<Pt::Char> _ops;
        std::stack<Fragment> _fragments;
};


class DtdParser : private Pt::NonCopyable
{
    public:
        DtdParser()
        : _state(&DtdParser::OnBegin)
        , _dtdBuilder(_dtd)
        {}

        ~DtdParser()
        { }

        void parse(const Pt::String& elem, const char* elemDecl)
        {
            _dtdBuilder.clear();
            _state = &DtdParser::OnBegin;

            DtdElementDecl& decl = _dtd.addElementDecl(elem);
            
            while(*elemDecl != '\0')
            {
                (this->*_state)(*elemDecl++);
            }

            (this->*_state)( std::char_traits<Pt::Char>::eof() );
            
            DtdElementDecl::Node& contentDecl = _dtdBuilder.finish();
            decl.setContentExpr(contentDecl);
        }

        DocTypeDefinition& dtd()
        { return _dtd; }

    private:
        Pt::Char notEof(int c) const
        {
            if( c == std::char_traits<Pt::Char>::eof() )
            {
                throw std::logic_error("DTD syntax error: unexpected EOF");
            }

            return Pt::Char(c);
        }

        bool isAlpha(Pt::Char ch)
        {
            return ch == '.' || ch == '_' || ch == '-' || Pt::isalnum(ch) != 0;
        }

        void OnBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'E' || ch == 'A')
            {
                _token.clear();
                _token += ch;
                _state = &DtdParser::OnEmptyOrAny;
                return;
            }

            if(ch != '(')
                throw std::logic_error("DTD syntax error: expected open brace");

            _dtdBuilder.pushOperator(ch);
            _state = &DtdParser::OnExprBegin;
        }
        
        void OnEmptyOrAny(int c)
        {
            if( c == std::char_traits<Pt::Char>::eof() )
            {
                if(_token == L"EMPTY")
                {
                    _dtdBuilder.pushEmpty();
                    return;
                }
            }
            else
            {
                Pt::Char ch(c);
                if( isAlpha(ch) )
                {
                    _token += ch;
                    return;
                }
            }

            throw std::logic_error("DTD syntax error: expected EMPTY");
        }

        void OnExprBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token.clear();
                _token += ch;
                _state = &DtdParser::OnIdentifier;
                return;
            }

            if(ch == '(')
            {
                _dtdBuilder.pushOperator(ch);
                return;
            }

            throw std::logic_error("DTD syntax error");
        }

        void OnIdentifier(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _state = &DtdParser::OnIdentifier;
                return;
            }

            if( ch == ',')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushClosingBrace();
                _state = &DtdParser::OnExprEnd;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid identifier");
        }
        
        void OnUnrayOp(int c)
        {
            if( c == std::char_traits<Pt::Char>::eof() )
            {
                return;
            }

            Pt::Char ch(c);

            if( ch == ',')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == ')')
            {
                _dtdBuilder.pushClosingBrace();
                _state = &DtdParser::OnExprEnd;
                return;
            }
        }
        
        void OnBinaryOp(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token.clear();
                _token += ch;
                _state = &DtdParser::OnIdentifier;
                return;
            }

            if(ch == '(')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnExprBegin;
                return;
            }

            throw std::logic_error("DTD syntax error");
        }

        void OnExprEnd(int c)
        {
            if( c == std::char_traits<Pt::Char>::eof() )
            {
                return;
            }

            Pt::Char ch(c);

            if( ch == ',')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _dtdBuilder.pushClosingBrace();
                return;
            }
        }

    private:
        typedef void (DtdParser::*ParseFunc)(int);
        ParseFunc _state;
        Pt::String _token;

        DocTypeDefinition _dtd;
        DtdElementDeclBuilder _dtdBuilder;
};


class DtdElementValidator
{
    public:
        DtdElementValidator()
        : _decl(0)
        {}

        DtdElementValidator(DtdElementDecl& decl)
        : _decl(&decl)
        {
            _decl->startNode()->get(_current);
        }

        bool validateAttributes(Pt::Xml::AttributeList& attrs)
        {
            return _decl ? _decl->validate(attrs) : false;
        }

        bool validateContent(Pt::Xml::Node& node)
        {
            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdElementDecl::Node* state = _current[n];
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
        DtdElementDecl* _decl;
        std::vector<DtdElementDecl::Node*> _current;
        std::vector<DtdElementDecl::Node*> _next;
};


class DtdValidator
{
    public:
        DtdValidator(DocTypeDefinition& dtd)
        : _dtd(&dtd)
        {}

        ~DtdValidator()
        {}

        bool validate(Pt::Xml::Node& node)
        {
            bool valid = true;
            
            // TODO: ignorable whitespace

            switch( node.type() )
            {
                case Pt::Xml::Node::StartElement:
                {
                    Pt::Xml::StartElement& se = static_cast<Pt::Xml::StartElement&>(node);

                    if( ! _decls.empty() )
                    {
                        valid = _decls.top().validateContent(se);
                    }
                    
                    DtdElementDecl* decl = _dtd->findElementDecl( se.name() );
                    if(decl)
                    {
                        _decls.push( DtdElementValidator(*decl) );
                    }
                    else
                    {
                        _decls.push( DtdElementValidator() );
                    }

                    valid = valid && _decls.top().validateAttributes( se.attributes() );
                    break;
                }
                
                case Pt::Xml::Node::Characters:
                {
                    Pt::Xml::Characters& chars = static_cast<Pt::Xml::Characters&>(node);

                    if( ! _decls.empty() )
                    {
                        valid = _decls.top().validateContent(chars);
                    }

                    break;
                }

                case Pt::Xml::Node::EndElement:
                {
                    valid = _decls.top().isValid();
                    _decls.pop();
                    break;
                }
            }

            return valid;
        }

    private:
        DocTypeDefinition* _dtd;
        std::stack<DtdElementValidator> _decls;
};


class PtXmlTest : public Pt::Unit::TestSuite
{
    public:
        PtXmlTest()
        : Pt::Unit::TestSuite("Pt-Xml-Test")
        {
            this->registerMethod("DtdValidate", *this, &PtXmlTest::DtdValidate);
            this->registerMethod("DtdValidateAttributes", *this, &PtXmlTest::DtdValidateAttributes);
        }

    private:
        void DtdValidate()
        {
            std::istringstream text("<test><a>hello</a><b></b><a>world</a><b></b></test>");
            //std::istringstream text("<test></test>");

            DtdParser parser;
            parser.parse(L"test", "(a|b)+");
            parser.parse(L"a", "(#PCDATA|(x|y)?|z+)");
            parser.parse(L"b", "EMPTY");

            DtdValidator validator( parser.dtd() );

            Pt::Xml::XmlReader reader(text);
            for(;;)
            {
                Pt::Xml::Node& node = reader.next();
                bool valid = validator.validate(node);

                if( ! valid)
                    this->reportMessage("DTD VALIDATION FAILED");

                if( node.type() == Pt::Xml::Node::EndDocument )
                    break;
            }
        }

        void DtdValidateAttributes()
        {
            DtdAttrListDecl decl;

            DtdAttrCDataDecl* a1 = new DtdAttrCDataDecl();
            a1->setName(L"a1");
            a1->setMode(DtdAttrDecl::Required);
            decl.push(a1);

            DtdAttrCDataDecl* a2 = new DtdAttrCDataDecl();
            a2->setName(L"a2");
            a2->setMode(DtdAttrDecl::Implied);
            decl.push(a2);

            DtdAttrCDataDecl* a3 = new DtdAttrCDataDecl();
            a3->setName(L"a3");
            a3->setMode(DtdAttrDecl::Default);
            a3->setDefault(L"a3Def");
            decl.push(a3);

            Pt::Xml::Attribute attr;
            Pt::Xml::AttributeList attrs;

            attr.setName(L"a1");
            attr.setValue(L"a1Val");
            attrs.add(attr);

            //attr.setName(L"a2");
            //attr.setValue(L"a2Val");
            //attrs.add(attr);

            //attr.setName(L"a3");
            //attr.setValue(L"a3Val");
            //attrs.add(attr);

            bool isMatch = decl.validate(attrs);
            this->reportMessage(isMatch ? "DTD ATTRIBUTE MATCH" : "INVALID ATTRIBUTE");
            
            Pt::Xml::AttributeList::Iterator it;
            for(it = attrs.begin(); it != attrs.end(); ++it)
            {
                this->reportMessage( it->name().narrow() + ": " + it->value().narrow() );
            }
        }
};

Pt::Unit::RegisterTest<PtXmlTest> register_PtXmlTest;
