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


class DtdState
{
    public:
        virtual ~DtdState()
        { }

        virtual void next(std::vector<DtdState*>& states) = 0;

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states) = 0;

        virtual bool isValid() const
        { return false; }

        DtdState* out()
        { return _out; }

        void setNext(DtdState* state)
        { _out = state; }

    protected:
        DtdState()
        : _out(0)
        {}

    private:
        DtdState* _out;
};


class DtdSplit : public DtdState
{
    public:
        DtdSplit()
        : DtdState()
        , _out1(0)
        { }

        void setSplit(DtdState* state)
        { _out1 = state; }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }

        virtual void next(std::vector<DtdState*>& states) 
        {
            assert(out());
            out()->next(states);

            assert(_out1);
            _out1->next(states);
        }

    private:
        DtdState* _out1;
};


class DtdLabel : public DtdState
{
    public:
        DtdLabel(const Pt::String& name)
        : DtdState()
        , _name(name)
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        {
            Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&node);
            if(se && se->name() == _name)
                out()->next(states);
        }

        virtual void next(std::vector<DtdState*>& states) 
        {
            states.push_back(this);
        }

    private:
        Pt::String _name;
};


class DtdMatch : public DtdState
{
    public:
        DtdMatch()
        : DtdState()
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }
        
        virtual void next(std::vector<DtdState*>& states) 
        { }

        virtual bool isValid() const
        { return true; }
};


class DtdEnd : public DtdState
{
    public:
        DtdEnd()
        : DtdState()
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        {
            Pt::Xml::EndElement* e = Pt::Xml::toEndElement(&node);
            if(e != 0)
                states.push_back(&_end);
        }
        
        virtual void next(std::vector<DtdState*>& states) 
        { states.push_back(this); }

        DtdState* matched()
        { return &_end; }

    private:
        DtdMatch _end;
};


class DtdFragment
{
    public:
        explicit DtdFragment(DtdState* start)
        : _start(start)
        {}

        DtdState* start() const
        { return _start; }

        const std::vector<DtdState*>& leafs() const
        { return _leafs; }

        void setLeaf(DtdState* next)
        { _leafs.push_back(next); }

        void setLeafs(const std::vector<DtdState*>& leafs)
        { _leafs = leafs; }

        void setLeafs(const std::vector<DtdState*>& leafs, const std::vector<DtdState*>& leafs2)
        { 
            _leafs = leafs; 
            _leafs.insert( _leafs.end(), leafs2.begin(), leafs2.end() );
        }

        void setLeafs(const std::vector<DtdState*>& leafs, DtdState* leaf)
        { 
            _leafs = leafs; 
            _leafs.push_back(leaf);
        }

        void patchLeafs(DtdState* to)
        {
            for(unsigned n = 0; n < _leafs.size(); ++n)
            {
                DtdState* leaf = _leafs[n];
                leaf->setNext(to);
            }
        }

    private:
        DtdState* _start;
        std::vector<DtdState*> _leafs;
};


class DtdElementDecl
{
    // nested classes: State. Split, Label, Match, End...

    public:
        DtdElementDecl()
        : _start(0)
        , _attr(0)
        {}

        void setContentDecl(DtdState& s)
        { _start = &s; }

        void setAttrListDecl(DtdAttrListDecl& attr)
        { _attr = &attr; }

        bool validate(Pt::Xml::AttributeList& attrs)
        {
            return _attr ? _attr->validate(attrs) : attrs.empty();
        }

        void begin(std::vector<DtdState*>& states)
        {
            _start->next(states);
        }

    private:
        DtdState* _start;
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

        DtdLabel* createLabel(const Pt::String& name)
        {
            _pool.reserve(_pool.size() + 1);
            DtdLabel* label = new DtdLabel(name);
            _pool.push_back(label);
            return label;
        }

        DtdSplit* createSplit()
        {
            _pool.reserve(_pool.size() + 1);
            DtdSplit* split = new DtdSplit();
            _pool.push_back(split);
            return split;
        }

        DtdElementDecl& elementDecl(const Pt::String& name)
        { 
            return _elemDecls[name]; 
        }

        DtdState* matched()
        {
            return _dtdEnd.matched();
        }

        DtdState* createEnd()
        { return &_dtdEnd; }

    private:
        DtdEnd _dtdEnd;
        std::vector<DtdState*> _pool;
        std::map<Pt::String, DtdElementDecl> _elemDecls;
};


class DtdElementDeclBuilder
{
    // nested classes: Fragment

    public:
        DtdElementDeclBuilder(DocTypeDefinition& dtd)
        : _dtd(&dtd)
        {}

        DtdState* finish()
        {
            reduceStack();

            if(_fragments.size() != 1)
                throw std::logic_error("DTD syntax error: incomplete expression");

            _fragments.top().patchLeafs( _dtd->createEnd() );
            DtdState* start = _fragments.top().start();
            _fragments.pop();
            return start;
        }

        void pushOperator(Pt::Char ch)
        {
            _ops.push(ch);
        }
        
        void pushOperand(const Pt::String name)
        {
            DtdLabel* label = _dtd->createLabel(name);
            DtdFragment frag(label);
            frag.setLeaf(label);
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
                    
                    DtdFragment op2 = _fragments.top();
                    _fragments.pop();

                    DtdFragment op1 = _fragments.top();
                    _fragments.pop();

                    op1.patchLeafs( op2.start() );
                    
                    DtdFragment frag( op1.start() );
                    frag.setLeafs( op2.leafs() );
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '|')
                {
                    _ops.pop();
                 
                    if( _fragments.size() < 2 )
                        throw std::logic_error("DTD syntax error: not enough operands for ,");
                    
                    DtdFragment op2 = _fragments.top();
                    _fragments.pop();

                    DtdFragment op1 = _fragments.top();
                    _fragments.pop();

                    DtdSplit* split = _dtd->createSplit();
                    split->setNext( op1.start() );
                    split->setSplit( op2.start() );

                    DtdFragment frag(split);
                    frag.setLeafs( op1.leafs(), op2.leafs() );
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '?')
                {
                    _ops.pop();
                 
                    if( _fragments.empty() )
                        throw std::logic_error("DTD syntax error: not enough operands for ?");
                    
                    DtdFragment op1 = _fragments.top();
                    _fragments.pop();

                    DtdSplit* split = _dtd->createSplit();
                    split->setSplit( op1.start() );
                    
                    DtdFragment frag(split);
                    frag.setLeafs(op1.leafs(), split);
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '*')
                {
                    _ops.pop();
                 
                    if( _fragments.empty() )
                        throw std::logic_error("DTD syntax error: not enough operands for *");
                    
                    DtdFragment op1 = _fragments.top();
                    _fragments.pop();

                    DtdSplit* split = _dtd->createSplit();
                    split->setSplit( op1.start() );

                    op1.patchLeafs(split);
                    
                    DtdFragment frag( split );
                    frag.setLeaf(split);
                    _fragments.push(frag);
                    continue;
                }

                if(_ops.top() == '+')
                {
                    _ops.pop();
                 
                    if( _fragments.empty() )
                        throw std::logic_error("DTD syntax error: not enough operands for +");
                    
                    DtdFragment op1 = _fragments.top();
                    _fragments.pop();

                    DtdSplit* split = _dtd->createSplit();
                    split->setSplit( op1.start() );

                    op1.patchLeafs(split);
                    
                    DtdFragment frag( op1.start() );
                    frag.setLeaf(split);
                    _fragments.push(frag);
                    continue;
                }
            }
        }

    private:
        DocTypeDefinition* _dtd;
        std::stack<Pt::Char> _ops;
        std::stack<DtdFragment> _fragments;
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

        void parse(int c)
        {
            (this->*_state)(c);
        }

        DtdElementDecl& finish()
        {
            parse( std::char_traits<Pt::Char>::eof() );
            DtdState* contentDecl = _dtdBuilder.finish();

            DtdElementDecl& decl = _dtd.elementDecl(L"");
            decl.setContentDecl(*contentDecl);

            return decl;
        }

        DtdState* matched()
        {
            return _dtd.matched();
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

        void OnBegin(int c)
        {
            Pt::Char ch = notEof(c);
            if(ch != '(')
                throw std::logic_error("DTD syntax error: expected open brace");

            _dtdBuilder.pushOperator(ch);
            _state = &DtdParser::OnExprBegin;
        }

        void OnExprBegin(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token.clear();
                _token += ch;
                _state = &DtdParser::OnIdentifier;
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

            if(ch == '+')
            {
                _dtdBuilder.pushOperator(ch);
                return;
            }

            if(ch == '*')
            {
                _dtdBuilder.pushOperator(ch);
                return;
            }

            if(ch == '?')
            {
                _dtdBuilder.pushOperator(ch);
                return;
            }
        }

        void OnIdentifier(int c)
        {
            Pt::Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                _token.clear();
                _token += ch;
                _state = &DtdParser::OnIdentifier;
                return;
            }

            if( ch == ',')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnExprBegin;
                return;
            }

            if( ch == '|')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.pushOperator(ch);
                _state = &DtdParser::OnExprBegin;
                return;
            }

            if( ch == ')')
            {
                _dtdBuilder.pushOperand(_token);
                _dtdBuilder.reduceStack();
                _state = &DtdParser::OnExprEnd;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid identifier");
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
        DtdElementValidator(DtdElementDecl& decl)
        : _decl(&decl)
        {
            _decl->begin(_current);
        }

        bool begin(Pt::Xml::AttributeList& attrs)
        {
            _decl->validate(attrs);
        }

        void advance(Pt::Xml::Node& node)
        {
            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->eval(node, _next);
            }
            
            _current = _next;
            _next.clear();
        }

        bool isValid() const
        { return ! _current.empty() && _current[0]->isValid(); }

    private:
        DtdElementDecl* _decl;
        std::vector<DtdState*> _current;
        std::vector<DtdState*> _next;
};


class DtdValidator
{
    public:
        DtdValidator(DocTypeDefinition& dtd)
        : _dtd(&dtd)
        {}

        ~DtdValidator()
        {}

        void processNode(Pt::Xml::Node& node)
        {}

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
            DtdParser parser;
            parser.parse('(');
            parser.parse('a');
            parser.parse('|');
            parser.parse('b');
            parser.parse(')');
            parser.parse('*');
            DtdElementValidator validator( parser.finish() );

            // first token <a>
            Pt::Xml::StartElement seA;
            seA.setName(L"a");
            validator.advance(seA);

            // second token <b>
            Pt::Xml::StartElement seB;
            seB.setName(L"b");
            validator.advance(seB);

            // third token <a>
            validator.advance(seA);

            // fourth token <b>
            validator.advance(seB);

            // 'end of input' token
            Pt::Xml::EndElement endElem;
            validator.advance(endElem);

            bool isMatch = validator.isValid();
            this->reportMessage(isMatch ? "DTD ELEMENT MATCH" : "INVALID ELEMENT");
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
