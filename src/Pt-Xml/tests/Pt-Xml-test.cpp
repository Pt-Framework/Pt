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

        void setDefaultValue(const Pt::String& def)
        { _default = def; }

        const Pt::String& defaultValue() const
        { return _default; }

        bool validate(const Pt::String& value) const
        {
            if(mode() == Fixed)
                return value == defaultValue();

            return onValidate(value);
        }
      
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

    protected:
        virtual bool onValidate(const Pt::String& value) const = 0;

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

        virtual bool onValidate(const Pt::String& value) const
        { 
            // TODO: check for non-CDATA characters in value           
            return true; 
        }
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

        DtdAttrDecl& last()
        {
            return *(_attrs.back());
        }

        void push(DtdAttrDecl* decl)
        { _attrs.push_back(decl); }

        bool validate(Pt::Xml::AttributeList& attrs) const
        {
            std::vector<DtdAttrDecl*>  attrDecls = _attrs;
            //
            // match attributes against declarations, remove declarations
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

                if( ! (*it)->validate( attr->value() ) )
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

class DtdElementContentDecl
{
    public:
        class Node
        {
            public:
                virtual ~Node()
                { }

                //! @brief Gets this node and follows unlabelled transitions.
                virtual void get(std::vector<Node*>& nodes) = 0;

                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& states) = 0;

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

        class Fragment
        {
            public:
                explicit Fragment(DtdElementContentDecl::Node& start)
                : _start(&start)
                {}

                DtdElementContentDecl::Node& start() const
                { return *_start; }

                const std::vector<DtdElementContentDecl::Node*>& leafs() const
                { return _leafs; }

                void setLeaf(DtdElementContentDecl::Node& next)
                { _leafs.push_back(&next); }

                void setLeafs(const std::vector<DtdElementContentDecl::Node*>& leafs)
                { _leafs = leafs; }

                void setLeafs(const std::vector<DtdElementContentDecl::Node*>& leafs, const std::vector<DtdElementContentDecl::Node*>& leafs2)
                { 
                    _leafs = leafs; 
                    _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
                }

                void setLeafs(const std::vector<DtdElementContentDecl::Node*>& leafs, DtdElementContentDecl::Node& leaf)
                { 
                    _leafs = leafs; 
                    _leafs.push_back(&leaf);
                }

                void patchLeafs(DtdElementContentDecl::Node& to)
                {
                    for(unsigned n = 0; n < _leafs.size(); ++n)
                    {
                        DtdElementContentDecl::Node* leaf = _leafs[n];
                        leaf->setNext(to);
                    }
                }

            private:
                Node* _start;
                std::vector<Node*> _leafs;
        };

        class FragmentStack
        {
            public:
                FragmentStack()
                : _decl(0)
                {}
                
                void reset(DtdElementContentDecl& decl)
                {
                    while( ! _fragments.empty() )
                        _fragments.pop();
                    
                    while( ! _ops.empty() )
                        _ops.pop();

                    _decl = &decl;
                }

                void finish()
                {
                    reduceStack();

                    if(_fragments.size() != 1)
                        throw std::logic_error("DTD syntax error: incomplete expression");

                    _fragments.top().patchLeafs( _decl->createEnd() );
                    _decl->setStart( _fragments.top().start() );
                    _fragments.pop();
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

                        DtdElementContentDecl::PcData& pcdata = _decl->createPcData();
                        DtdElementContentDecl::Fragment frag(pcdata);
                        frag.setLeaf(pcdata);
                        _fragments.push(frag);
                        return;
                    }
                
                    DtdElementContentDecl::Label& label =_decl->createLabel(name);
                    DtdElementContentDecl::Fragment frag(label);
                    frag.setLeaf(label);
                    _fragments.push(frag);
                }

                void pushEmpty()
                {
                    DtdElementContentDecl::Empty& e = _decl->createEmpty();
                    DtdElementContentDecl::Fragment frag(e);
                    frag.setLeaf(e);
                    _fragments.push(frag);
                }

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
                    
                            DtdElementContentDecl::Fragment op2 = _fragments.top();
                            _fragments.pop();

                            DtdElementContentDecl::Fragment op1 = _fragments.top();
                            _fragments.pop();

                            op1.patchLeafs( op2.start() );
                    
                            DtdElementContentDecl::Fragment frag( op1.start() );
                            frag.setLeafs( op2.leafs() );
                            _fragments.push(frag);
                            continue;
                        }

                        if(_ops.top() == '|')
                        {
                            _ops.pop();
                 
                            if( _fragments.size() < 2 )
                                throw std::logic_error("DTD syntax error: not enough operands for ,");
                    
                            DtdElementContentDecl::Fragment op2 = _fragments.top();
                            _fragments.pop();

                            DtdElementContentDecl::Fragment op1 = _fragments.top();
                            _fragments.pop();

                            DtdElementContentDecl::Split& split = _decl->createSplit( op2.start() );
                            split.setNext( op1.start() );

                            DtdElementContentDecl::Fragment frag(split);
                            frag.setLeafs( op1.leafs(), op2.leafs() );
                            _fragments.push(frag);
                            continue;
                        }

                        if(_ops.top() == '?')
                        {
                            _ops.pop();
                 
                            if( _fragments.empty() )
                                throw std::logic_error("DTD syntax error: not enough operands for ?");
                    
                            DtdElementContentDecl::Fragment op1 = _fragments.top();
                            _fragments.pop();

                            DtdElementContentDecl::Split& split = _decl->createSplit( op1.start() );
                    
                            DtdElementContentDecl::Fragment frag(split);
                            frag.setLeafs(op1.leafs(), split);
                            _fragments.push(frag);
                            continue;
                        }

                        if(_ops.top() == '*')
                        {
                            _ops.pop();
                 
                            if( _fragments.empty() )
                                throw std::logic_error("DTD syntax error: not enough operands for *");
                    
                            DtdElementContentDecl::Fragment op1 = _fragments.top();
                            _fragments.pop();

                            DtdElementContentDecl::Split& split = _decl->createSplit( op1.start() );

                            op1.patchLeafs(split);
                    
                            DtdElementContentDecl::Fragment frag( split );
                            frag.setLeaf(split);
                            _fragments.push(frag);
                            continue;
                        }

                        if(_ops.top() == '+')
                        {
                            _ops.pop();
                 
                            if( _fragments.empty() )
                                throw std::logic_error("DTD syntax error: not enough operands for +");
                    
                            DtdElementContentDecl::Fragment op1 = _fragments.top();
                            _fragments.pop();

                            DtdElementContentDecl::Split& split = _decl->createSplit( op1.start() );

                            op1.patchLeafs(split);
                    
                            DtdElementContentDecl::Fragment frag( op1.start() );
                            frag.setLeaf(split);
                            _fragments.push(frag);
                            continue;
                        }
                    }
                }

            private:
                DtdElementContentDecl* _decl;
                std::stack<Pt::Char> _ops;
                std::stack<Fragment> _fragments;
        };

    public:
        DtdElementContentDecl()
        : _start(0)
        {}

        ~DtdElementContentDecl()
        {
            for(unsigned n = 0; n < _pool.size() ; ++n)
            {
                delete _pool[n];
            }
        }

        Node* start()
        { return _start; }

        void setStart(Node& node)
        { _start = &node; }

        Label& createLabel(const Pt::String& name)
        {
            _pool.reserve(_pool.size() + 1);
            Label* label = new Label(name);
            _pool.push_back(label);
            return *label;
        }

        Split& createSplit(Node& to)
        {
            _pool.reserve(_pool.size() + 1);
            Split* split = new Split(&to);
            _pool.push_back(split);
            return *split;
        }

        PcData& createPcData()
        {
            _pool.reserve(_pool.size() + 1);
            PcData* node = new PcData();
            _pool.push_back(node);
            return *node;
        }

        Empty& createEmpty()
        { return _dtdEmpty; }

        Node& createEnd()
        { return _dtdEnd; }

    private:
        std::vector<Node*> _pool;
        Empty _dtdEmpty;
        Match _dtdEnd;
        Node* _start;
};


class DtdElementDecl
{
    public:
        DtdElementDecl()
        {}

        DtdElementContentDecl& contentDecl()
        { return _content; }

        DtdAttrListDecl& attrListDecl()
        { return _attr; }

    private:
        DtdElementContentDecl _content;
        DtdAttrListDecl _attr;
};


class DocTypeDefinition : private Pt::NonCopyable
{
    public:
        DocTypeDefinition()
        {}

        ~DocTypeDefinition()
        {}

        DtdElementDecl& declareElement(const Pt::String& name)
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
        std::map<Pt::String, DtdElementDecl> _elemDecls;
};


class DtdParser : private Pt::NonCopyable
{
    public:
        DtdParser(DocTypeDefinition& dtd)
        : _dtd(dtd)
        , _state(&DtdParser::OnBegin)
        , _elemDecl(0)
        {}

        ~DtdParser()
        { }

        void parseElementDecl(const Pt::String& elem, const char* elemDecl)
        {
            _token.clear();
            _state = &DtdParser::OnBegin;

            DtdElementDecl& decl = _dtd.declareElement(elem);
            _elemDecl = &decl;

            _fragments.reset( decl.contentDecl() );

            while(*elemDecl != '\0')
                (this->*_state)(*elemDecl++);

            (this->*_state)( std::char_traits<Pt::Char>::eof() );

            _fragments.finish();
        }

        void parseAttrDecl(const Pt::String& elem, const char* attrDecl)
        {
            _token.clear();
            _state = &DtdParser::OnAttrName;

            DtdElementDecl& decl = _dtd.declareElement(elem);
            _elemDecl = &decl;

            while(*attrDecl != '\0')
                (this->*_state)(*attrDecl++);

            (this->*_state)( std::char_traits<Pt::Char>::eof() );
        }

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

        void OnAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _state = &DtdParser::AfterAttrName;
                return;
            }

            _token += ch;
        }

        void AfterAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == 'C' )
            {
                _state = &DtdParser::OnCDATA0;
                return;
            }
        }

        void OnCDATA0(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'D' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnCDATA1;
        }

        void OnCDATA1(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'A' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnCDATA2;
        }

        void OnCDATA2(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'T' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnCDATA3;
        }

        void OnCDATA3(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'A' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            DtdAttrCDataDecl* attr = new DtdAttrCDataDecl();
            attr->setName(_token);
            _elemDecl->attrListDecl().push(attr);

            _token.clear();
            _state = &DtdParser::AfterAttrType;
        }

        void AfterAttrType(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
                return;

            if(ch == '"')
            {
                _state = &DtdParser::OnAttrDefault;
                return;
            }

            if( ch != '#' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnAttrMode;
        }

        void OnAttrMode(int c)
        {
            Pt::Char ch(c);
            if( c == std::char_traits<Pt::Char>::eof() || Pt::isspace(ch) )
            {
                if(_token == L"REQUIRED")
                {
                    _elemDecl->attrListDecl().last().setMode(DtdAttrDecl::Required);
                    _state = &DtdParser::AfterAttrMode;
                }
                else if(_token == L"IMPLIED")
                {
                    _elemDecl->attrListDecl().last().setMode(DtdAttrDecl::Implied);
                    _state = &DtdParser::AfterAttrMode;
                }
                else if(_token == L"FIXED")
                {
                    _elemDecl->attrListDecl().last().setMode(DtdAttrDecl::Fixed);
                    _state = &DtdParser::AfterAttrFixed;
                }
                else
                    throw std::logic_error("DTD syntax error: invalid attribute mode");
                
                _token.clear();
                return;
            }

            if( ! isAlpha(ch) )
                throw std::logic_error("DTD syntax error: expected attribute mode");

            _token += ch;
        }

        void AfterAttrMode(int c)
        {
            if( c == std::char_traits<Pt::Char>::eof() )
                return;

            Pt::Char ch(c);
            
            if( Pt::isspace(ch) )
                return;
        }

        void AfterAttrFixed(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '"')
            {
                _state = &DtdParser::OnAttrDefault;
                return;
            }

            if( Pt::isspace(ch) )
                return;

            throw std::logic_error("DTD syntax error: expected attribute default");
        }

        void OnAttrDefault(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '"')
            {
                _elemDecl->attrListDecl().last().setDefaultValue(_token);
                _token.clear();
                _state = &DtdParser::AfterAttrMode;
                return;
            }

            _token += ch;
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

            _fragments.pushOperator(ch);
            _state = &DtdParser::OnExprBegin;
        }
        
        void OnEmptyOrAny(int c)
        {
            if( c == std::char_traits<Pt::Char>::eof() )
            {
                if(_token == L"EMPTY")
                {
                    _fragments.pushEmpty();
                    _token.clear();
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
                _fragments.pushOperator(ch);
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
                _fragments.pushOperand(_token);
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _fragments.pushOperand(_token);
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _fragments.pushOperand(_token);
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _fragments.pushOperand(_token);
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _fragments.pushOperand(_token);
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _fragments.pushOperand(_token);
                _fragments.pushClosingBrace();
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
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == ')')
            {
                _fragments.pushClosingBrace();
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
                _fragments.pushOperator(ch);
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
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _fragments.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _fragments.pushClosingBrace();
                return;
            }
        }

    private:
        DocTypeDefinition& _dtd;

        typedef void (DtdParser::*ParseFunc)(int);
        ParseFunc _state;
        Pt::String _token;

        DtdElementDecl* _elemDecl;
        DtdElementContentDecl::FragmentStack _fragments;
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
            _decl->contentDecl().start()->get(_current);
        }

        bool validateAttributes(Pt::Xml::AttributeList& attrs)
        {
            return _decl ? _decl->attrListDecl().validate(attrs) : false;
        }

        bool validateContent(Pt::Xml::Node& node)
        {
            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdElementContentDecl::Node* state = _current[n];
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
        std::vector<DtdElementContentDecl::Node*> _current;
        std::vector<DtdElementContentDecl::Node*> _next;
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
            this->registerMethod("DtdValidateElementContent", *this, &PtXmlTest::DtdValidateElementContent);
            this->registerMethod("DtdValidateAttributes", *this, &PtXmlTest::DtdValidateAttributes);
        }

    private:
        void DtdValidateElementContent()
        {
            DocTypeDefinition dtd;

            DtdParser parser(dtd);
            parser.parseElementDecl(L"test", "(a|b)+");
            parser.parseElementDecl(L"a", "(#PCDATA|(x|y)?|z+)");
            parser.parseElementDecl(L"b", "EMPTY");

            DtdValidator validator(dtd);

            std::istringstream text("<test><a>hello</a><b></b><a>world</a><b></b></test>");
            Pt::Xml::XmlReader reader(text);
            
            Pt::Xml::XmlReader::Iterator it;
            for(it = reader.current(); it != reader.end(); ++it)
            {
                Pt::Xml::Node& node = *it;
                bool valid = validator.validate(node);
                PT_UNIT_ASSERT(valid);
            }
        }

        void DtdValidateAttributes()
        {
            DocTypeDefinition dtd;

            DtdParser parser(dtd);
            parser.parseElementDecl(L"test", "EMPTY");
            parser.parseAttrDecl(L"test", "a1 CDATA #REQUIRED");
            parser.parseAttrDecl(L"test", "a2 CDATA #IMPLIED");
            parser.parseAttrDecl(L"test", "a3 CDATA #FIXED \"A3def\"");
            parser.parseAttrDecl(L"test", "a4 CDATA \"A4def\"");

            DtdValidator validator(dtd);

            std::istringstream text("<test a1=\"A1\" a2=\"A2\" a4=\"A3def\"></test>");
            Pt::Xml::XmlReader reader(text);

            Pt::Xml::XmlReader::Iterator it;
            for(it = reader.current(); it != reader.end(); ++it)
            {
                Pt::Xml::Node& node = *it;
                bool valid = validator.validate(node);
                PT_UNIT_ASSERT(valid);

                Pt::Xml::StartElement* se = toStartElement(&node);
                if(se && se->name() == L"test")
                {
                    PT_UNIT_ASSERT( se->attributes().has(L"a3") );
                    PT_UNIT_ASSERT( se->attributes().find(L"a3")->value() == L"A3def" );

                    PT_UNIT_ASSERT( se->attributes().has(L"a4") );
                }
            }
        }
};

Pt::Unit::RegisterTest<PtXmlTest> register_PtXmlTest;
