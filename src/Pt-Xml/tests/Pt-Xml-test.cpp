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

class AttributeDeclaration
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
        AttributeDeclaration()
        : _mode(Default)
        {}

        virtual ~AttributeDeclaration()
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

        bool match(const Pt::Xml::Attribute& attr) const
        {
            if(mode() == Fixed)
                return attr.value() == defaultValue();

            return onMatch(attr);
        }
      
        bool validate(Pt::Xml::AttributeList& list) const
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
        virtual bool onMatch(const Pt::Xml::Attribute& attr) const = 0;

    private:
        Mode _mode;
        Pt::String _name;
        Pt::String _default;
};


class CDataAttributeDeclaration : public AttributeDeclaration
{
    public:
        CDataAttributeDeclaration()
        : AttributeDeclaration()
        {}

        virtual bool onMatch(const Pt::Xml::Attribute& attr) const
        { 
            // TODO: check for non-CDATA characters in value           
            return true; 
        }
};


class AttributeListDeclaration
{
    typedef std::vector<AttributeDeclaration*> AttrDecls;

    public:
        AttributeListDeclaration()
        {}

        ~AttributeListDeclaration()
        {
            AttrDecls::iterator it;
            for(it = _attrs.begin(); it != _attrs.end(); ++it)
            {
                delete *it;
            }
        }

        AttributeDeclaration& last()
        {
            return *_attrs.back();
        }

        void push(AttributeDeclaration* decl)
        { _attrs.push_back(decl); }

        bool validate(Pt::Xml::AttributeList& attrs) const
        {
            std::vector<AttributeDeclaration*>  attrDecls = _attrs;

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

                if( ! (*it)->match( *attr) )
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
                if( ! (*decl)->validate(attrs) )
                    return false;
            }

            return true;
        }

    private:
        AttrDecls _attrs;
};

class ContentModel
{
    public:
        //TODO: ContentExpression, ContentParticle
        class Node
        {
            public:
                virtual ~Node()
                { }

                //! @brief Gets this node and follows unlabelled transitions.
                virtual void get(std::vector<Node*>& nodes) = 0;

                //! @brief Evaluate the XML node and get all following nodes.
                virtual void eval(Pt::Xml::Node& node, std::vector<Node*>& states) = 0;

                //! @brief Returns true if the node represents a match state.
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
        ContentModel()
        : _start(&_match)
        {}

        ~ContentModel()
        {
            for(unsigned n = 0; n < _pool.size() ; ++n)
            {
                delete _pool[n];
            }
        }

        void start(std::vector<Node*>& nodes)
        { 
            assert(_start);
            return _start->get(nodes); 
        }

        void setStart(Node& node)
        { _start = &node; }

        Label& getLabel(const Pt::String& name)
        {
            _pool.reserve(_pool.size() + 1);
            Label* label = new Label(name);
            _pool.push_back(label);
            return *label;
        }

        Split& getSplit(Node& to)
        {
            _pool.reserve(_pool.size() + 1);
            Split* split = new Split(&to);
            _pool.push_back(split);
            return *split;
        }

        PcData& getPcData()
        {
            _pool.reserve(_pool.size() + 1);
            PcData* node = new PcData();
            _pool.push_back(node);
            return *node;
        }

        Empty& getEmpty()
        { return _empty; }

        Node& getMatch()
        { return _match; }

    private:
        Node* _start;

        // TODO: move to ContentModelContext, DtdContent etc...
        std::vector<Node*> _pool;
        Empty _empty;
        Match _match;
};


class ContentModelFragment
{
    public:
        explicit ContentModelFragment(ContentModel::Node& start)
        : _start(&start)
        {}

        ContentModel::Node& start() const
        { return *_start; }

        const std::vector<ContentModel::Node*>& leafs() const
        { return _leafs; }

        void setLeaf(ContentModel::Node& next)
        { _leafs.push_back(&next); }

        void setLeafs(const std::vector<ContentModel::Node*>& leafs)
        { _leafs = leafs; }

        void setLeafs(const std::vector<ContentModel::Node*>& leafs, const std::vector<ContentModel::Node*>& leafs2)
        { 
            _leafs = leafs; 
            _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
        }

        void setLeafs(const std::vector<ContentModel::Node*>& leafs, ContentModel::Node& leaf)
        { 
            _leafs = leafs; 
            _leafs.push_back(&leaf);
        }

        void patchLeafs(ContentModel::Node& to)
        {
            for(unsigned n = 0; n < _leafs.size(); ++n)
            {
                ContentModel::Node* leaf = _leafs[n];
                leaf->setNext(to);
            }
        }

    private:
        ContentModel::Node* _start;
        std::vector<ContentModel::Node*> _leafs;
};


class ContentModelBuilder
{
    public:
        ContentModelBuilder()
        : _decl(0)
        {}
                
        void reset(ContentModel& decl)
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

            if(_fragments.size() != 1 || _decl == 0)
                throw std::logic_error("DTD syntax error: incomplete expression");

            _fragments.top().patchLeafs( _decl->getMatch() );
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

                ContentModel::PcData& pcdata = _decl->getPcData();
                ContentModelFragment frag(pcdata);
                frag.setLeaf(pcdata);
                _fragments.push(frag);
                return;
            }
                
            ContentModel::Label& label =_decl->getLabel(name);
            ContentModelFragment frag(label);
            frag.setLeaf(label);
            _fragments.push(frag);
        }

        void pushEmpty()
        {
            ContentModel::Empty& e = _decl->getEmpty();
            ContentModelFragment frag(e);
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
                    
                    ContentModelFragment op2 = _fragments.top();
                    _fragments.pop();

                    ContentModelFragment op1 = _fragments.top();
                    _fragments.pop();

                    op1.patchLeafs( op2.start() );
                    
                    ContentModelFragment frag( op1.start() );
                    frag.setLeafs( op2.leafs() );
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '|')
                {
                    _ops.pop();
                 
                    if( _fragments.size() < 2 )
                        throw std::logic_error("DTD syntax error: not enough operands for ,");
                    
                    ContentModelFragment op2 = _fragments.top();
                    _fragments.pop();

                    ContentModelFragment op1 = _fragments.top();
                    _fragments.pop();

                    ContentModel::Split& split = _decl->getSplit( op2.start() );
                    split.setNext( op1.start() );

                    ContentModelFragment frag(split);
                    frag.setLeafs( op1.leafs(), op2.leafs() );
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '?')
                {
                    _ops.pop();
                 
                    if( _fragments.empty() )
                        throw std::logic_error("DTD syntax error: not enough operands for ?");
                    
                    ContentModelFragment op1 = _fragments.top();
                    _fragments.pop();

                    ContentModel::Split& split = _decl->getSplit( op1.start() );
                    
                    ContentModelFragment frag(split);
                    frag.setLeafs(op1.leafs(), split);
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '*')
                {
                    _ops.pop();
                 
                    if( _fragments.empty() )
                        throw std::logic_error("DTD syntax error: not enough operands for *");
                    
                    ContentModelFragment op1 = _fragments.top();
                    _fragments.pop();

                    ContentModel::Split& split = _decl->getSplit( op1.start() );

                    op1.patchLeafs(split);
                    
                    ContentModelFragment frag( split );
                    frag.setLeaf(split);
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '+')
                {
                    _ops.pop();
                 
                    if( _fragments.empty() )
                        throw std::logic_error("DTD syntax error: not enough operands for +");
                    
                    ContentModelFragment op1 = _fragments.top();
                    _fragments.pop();

                    ContentModel::Split& split = _decl->getSplit( op1.start() );

                    op1.patchLeafs(split);
                    
                    ContentModelFragment frag( op1.start() );
                    frag.setLeaf(split);
                    _fragments.push(frag);
                    continue;
                }
            }
        }

    private:
        ContentModel* _decl;
        std::stack<Pt::Char> _ops;
        std::stack<ContentModelFragment> _fragments;
};


class ElementDeclaration
{
    public:
        ElementDeclaration()
        {}

        ContentModel& contentModel()
        { return _content; }

        AttributeListDeclaration& attrListDecl()
        { return _attr; }

    private:
        ContentModel _content;
        AttributeListDeclaration _attr;
};


class DocTypeDefinition : private Pt::NonCopyable
{
    public:
        DocTypeDefinition()
        {}

        ~DocTypeDefinition()
        {}

        ElementDeclaration& declareElement(const Pt::String& name)
        { 
            return _elemDecls[name]; 
        }

        ElementDeclaration* findElementDecl(const Pt::String& name)
        {
            ElementDeclaration* decl = 0;

            std::map<Pt::String, ElementDeclaration>::iterator it;
            it = _elemDecls.find(name);

            if(it != _elemDecls.end())
                decl = &it->second;

            return decl; 
        }

    private:
        std::map<Pt::String, ElementDeclaration> _elemDecls;
};


class DtdParser : private Pt::NonCopyable
{
    public:
        DtdParser(DocTypeDefinition& dtd)
        : _dtd(dtd)
        , _state(&DtdParser::OnDtd)
        , _elemDecl(0)
        {}

        ~DtdParser()
        { }

        void parse(const char* elemDecl)
        {
            _elemDecl = 0;
            _token.clear();
            _state = &DtdParser::OnDtd;

            while(*elemDecl != '\0')
            {
                //std::cout << *elemDecl;
                (this->*_state)(*elemDecl++);
            }

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

        void OnDtd(int c)
        {
            if( c == std::char_traits<Pt::Char>::eof() )
                return;

            Pt::Char ch(c);

            if(ch == '<')
            {
                _state = &DtdParser::OnDtdTag;
                return;
            }
                
            if( Pt::isspace(ch) )
                return;

            throw std::logic_error("DTD syntax error: expected DTD entry1");
        }

        void OnDtdTag(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch != '!')
                throw std::logic_error("DTD syntax error: expected DTD entry2");

            _state = &DtdParser::OnDtdTagName;
        }

        void OnDtdTagName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                if(_token == L"ELEMENT")
                {
                    _state = &DtdParser::OnDtdElementBegin;
                    _token.clear();
                    return;
                }
                else if(_token == L"ATTLIST")
                {
                    _token.clear();
                    _state = &DtdParser::OnDtdAttListBegin;
                    return;
                }
            }

            throw std::logic_error("DTD syntax error: expected DTD entry3");
        }

        void OnDtdAttListBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _state = &DtdParser::OnDtdAttListName;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD attlist name");
        }

        void OnDtdAttListName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                _elemDecl = &_dtd.declareElement(_token);
                _token.clear();
                _state = &DtdParser::OnDtdBeforeAttrName;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD attlist name");
        }

        void OnDtdBeforeAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _state = &DtdParser::OnDtdAttrName;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD attribute name");
        }

        void OnDtdAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _state = &DtdParser::AfterDtdAttrName;
                return;
            }
            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD attribute name");
        }

        void AfterDtdAttrName(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == 'C' )
            {
                _state = &DtdParser::OnDtdCDATA0;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD attribute type");
        }

        void OnDtdCDATA0(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'D' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnDtdCDATA1;
        }

        void OnDtdCDATA1(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'A' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnDtdCDATA2;
        }

        void OnDtdCDATA2(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'T' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnDtdCDATA3;
        }

        void OnDtdCDATA3(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch != 'A' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            CDataAttributeDeclaration* attr = new CDataAttributeDeclaration();
            attr->setName(_token);
            _elemDecl->attrListDecl().push(attr);

            _token.clear();
            _state = &DtdParser::AfterDtdAttrType;
        }

        void AfterDtdAttrType(int c)
        {
            Pt::Char ch = notEof(c);

            if( Pt::isspace(ch) )
                return;

            if(ch == '"')
            {
                _state = &DtdParser::OnDtdAttrDefault;
                return;
            }

            if( ch != '#' )
                throw std::logic_error("DTD syntax error: expected attribute type");

            _state = &DtdParser::OnDtdAttrMode;
        }

        void OnDtdAttrMode(int c)
        {
            Pt::Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if(_token == L"REQUIRED")
            {
                _elemDecl->attrListDecl().last().setMode(AttributeDeclaration::Required);
                _state = &DtdParser::AfterDtdAttrMode;
            }
            else if(_token == L"IMPLIED")
            {
                _elemDecl->attrListDecl().last().setMode(AttributeDeclaration::Implied);
                _state = &DtdParser::AfterDtdAttrMode;
            }
            else if(_token == L"FIXED")
            {
                _elemDecl->attrListDecl().last().setMode(AttributeDeclaration::Fixed);
                _state = &DtdParser::AfterDtdAttrFixed;
            }
            else
                throw std::logic_error("DTD syntax error: invalid attribute mode");
                
            _token.clear();

            (this->*_state)(c);
        }

        void AfterDtdAttrMode(int c)
        {
            Pt::Char ch = notEof(c);

            if(c == '>')
            {
                _elemDecl = 0;
                _state = &DtdParser::OnDtd;
                return;
            }
            
            if( Pt::isspace(ch) )
                return;

            if( isAlpha(ch) )
            {
                _token += ch;
                _state = &DtdParser::OnDtdAttrName;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid attribute mode");
        }

        void AfterDtdAttrFixed(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '"')
            {
                _state = &DtdParser::OnDtdAttrDefault;
                return;
            }

            if( Pt::isspace(ch) )
                return;

            throw std::logic_error("DTD syntax error: expected attribute default");
        }

        void OnDtdAttrDefault(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == '"')
            {
                _elemDecl->attrListDecl().last().setDefaultValue(_token);
                _token.clear();
                _state = &DtdParser::AfterDtdAttrMode;
                return;
            }

            _token += ch;
        }

        void OnDtdElementBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                _state = &DtdParser::OnDtdElementName;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error: expected DTD element declaration");
        }

        void OnDtdElementName(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if( Pt::isspace(ch) )
            {
                _elemDecl = &_dtd.declareElement(_token);
                _token.clear();
                _builder.reset( _elemDecl->contentModel() );
                _state = &DtdParser::OnElementContentBegin;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD element declaration");
        }

        void OnElementContentBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if(ch == 'E' || ch == 'A')
            {
                _token.clear();
                _token += ch;
                _state = &DtdParser::OnEmptyOrAny;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch != '(')
                throw std::logic_error("DTD syntax error: expected open brace");

            _builder.pushOperator(ch);
            _state = &DtdParser::OnElementContent;
        }
        
        void OnEmptyOrAny(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token += ch;
                return;
            }

            if(_token == L"EMPTY")
            {
                _builder.pushEmpty();
            }
            else
                throw std::logic_error("DTD syntax error: invalid DTD element declaration");
            
            _token.clear();

            _state = &DtdParser::OnDtdBeforeElementEnd;
            (this->*_state)(c);
        }

        void OnDtdBeforeElementEnd(int c)
        {
            Pt::Char ch = notEof(c);
            
            if(ch == '>')
            {
                _builder.finish();
                _elemDecl = 0;
                _state = &DtdParser::OnDtd;
                return;
            }
            
            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error: invalid DTD element declaration");
        }

        void OnElementContent(int c)
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
                _builder.pushOperator(ch);
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
                _builder.pushOperand(_token);
                _token.clear();
                _builder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _builder.pushOperand(_token);
                _token.clear();
                _builder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _builder.pushOperand(_token);
                _token.clear();
                _builder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _builder.pushOperand(_token);
                _token.clear();
                _builder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _builder.pushOperand(_token);
                _token.clear();
                _builder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _builder.pushOperand(_token);
                _token.clear();
                _builder.pushClosingBrace();
                _state = &DtdParser::OnDtdContentExprEnd;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid identifier");
        }
        
        void OnUnrayOp(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                _builder.finish();
                _elemDecl = 0;
                _state = &DtdParser::OnDtd;
                return;
            }

            if( ch == ',')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == ')')
            {
                _builder.pushClosingBrace();
                _state = &DtdParser::OnDtdContentExprEnd;
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error");
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
                _builder.pushOperator(ch);
                _state = &DtdParser::OnElementContent;
                return;
            }

            throw std::logic_error("DTD syntax error");
        }

        void OnDtdContentExprEnd(int c)
        {
            Pt::Char ch = notEof(c);

            if( ch == '>' )
            {
                _builder.finish();
                _elemDecl = 0;
                _state = &DtdParser::OnDtd;
                return;
            }

            if( ch == ',')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if( ch == '|')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnBinaryOp;
                return;
            }

            if(ch == '+')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '*')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if(ch == '?')
            {
                _builder.pushOperator(ch);
                _state = &DtdParser::OnUnrayOp;
                return;
            }

            if( ch == ')')
            {
                _builder.pushClosingBrace();
                return;
            }

            if( Pt::isspace(ch) )
            {
                return;
            }

            throw std::logic_error("DTD syntax error");
        }

    private:
        DocTypeDefinition& _dtd;

        typedef void (DtdParser::*ParseFunc)(int);
        ParseFunc _state;
        Pt::String _token;

        ElementDeclaration* _elemDecl;
        ContentModelBuilder _builder;
};


class ElementValidator
{
    public:
        ElementValidator()
        : _decl(0)
        {}

        ElementValidator(ElementDeclaration& decl)
        : _decl(&decl)
        {
            _decl->contentModel().start(_current);
        }

        bool validateAttributes(Pt::Xml::AttributeList& attrs)
        {
            return _decl ? _decl->attrListDecl().validate(attrs) : false;
        }

        bool validateContent(Pt::Xml::Node& node)
        {
            for(unsigned n = 0; n < _current.size(); ++n)
            {
                ContentModel::Node* state = _current[n];
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
        ElementDeclaration* _decl;
        std::vector<ContentModel::Node*> _current;
        std::vector<ContentModel::Node*> _next;
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
                    
                    ElementDeclaration* decl = _dtd->findElementDecl( se.name() );
                    if(decl)
                    {
                        _decls.push( ElementValidator(*decl) );
                    }
                    else
                    {
                        _decls.push( ElementValidator() );
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
        std::stack<ElementValidator> _decls;
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
            parser.parse("<!ELEMENT test (a|b)+> "
                         "<!ELEMENT a (#PCDATA|(x|y)?|z+) >"
                         "<!ELEMENT b EMPTY>");

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
            parser.parse("<!ELEMENT test EMPTY>"
                         "<!ATTLIST test a1 CDATA #REQUIRED>"
                         "<!ATTLIST test a2 CDATA #IMPLIED"
                         "          a3 CDATA #FIXED \"A3def\""
                         "          a4 CDATA \"A4def\">");

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
