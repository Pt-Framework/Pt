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

        virtual void setNext(DtdState* state) = 0;

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states) = 0;

        DtdState* out()
        { return _out; }

        DtdState* out1()
        { return _out1; }

        void setOut(DtdState* next, DtdState* alt)
        {
            _out = next;
            _out1 = alt;
        }

    protected:
        DtdState()
        : _out(0)
        , _out1(0)
        {}

        DtdState(DtdState* next, DtdState* alt)
        : _out(next)
        , _out1(alt)
        {}

    private:
        DtdState* _out;
        DtdState* _out1;
};


class DtdOr : public DtdState
{
    public:
        DtdOr(DtdState* next, DtdState* alt)
        : DtdState(next, alt)
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }

        virtual void next(std::vector<DtdState*>& states) 
        {
            assert(out());
            out()->next(states);

            assert(out1());
            out1()->next(states);
        }

        virtual void setNext(DtdState* state)
        { 
            setOut(state, state);
        }
};


class DtdQuest : public DtdState
{
    public:
        DtdQuest(DtdState* next)
        : DtdState(next, 0)
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }

        virtual void next(std::vector<DtdState*>& states) 
        {
            assert( out() );
            out()->next(states);

            assert( out1() );
            out1()->next(states);
        }

        virtual void setNext(DtdState* state)
        { 
            setOut(out(), state);
        }
};


class DtdMult : public DtdState
{
    public:
        DtdMult(DtdState* next)
        : DtdState(next, 0)
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }

        virtual void next(std::vector<DtdState*>& states) 
        {
            assert( out() );
            out()->next(states);

            assert( out1() );
            out1()->next(states);
        }

        virtual void setNext(DtdState* state)
        { 
            setOut(out(), state);
        }
};


class DtdPlus : public DtdState
{
    public:
        DtdPlus(DtdState* prev)
        : DtdState(prev, 0)
        { }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }

        virtual void next(std::vector<DtdState*>& states) 
        {
            assert( out() );
            out()->next(states);

            assert( out1() );
            out1()->next(states);
        }

        virtual void setNext(DtdState* state)
        { 
            setOut(out(), state);
        }
};


class DtdElement : public DtdState
{
    public:
        DtdElement(DtdState* next, const Pt::String& name)
        : DtdState(next, 0)
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

        virtual void setNext(DtdState* state)
        { setOut(state, 0); }

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
        { states.push_back(this); }

        virtual void setNext(DtdState* state)
        { }
};


class DtdEnd : public DtdState
{
    public:
        DtdEnd()
        : DtdState()
        {
            setOut(&_end, 0);
        }

        virtual void eval(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        {
            Pt::Xml::EndElement* e = Pt::Xml::toEndElement(&node);
            if(e != 0)
                out()->next(states);
        }
        
        virtual void next(std::vector<DtdState*>& states) 
        {
            states.push_back(this);
        }

        virtual void setNext(DtdState* state)
        { }

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


class DtdParser : private Pt::NonCopyable
{
    public:
        DtdParser()
        : _state(&DtdParser::OnBegin)
        {}

        ~DtdParser()
        {
            for(unsigned n = 0; n < _expr.size() ; ++n)
            {
                delete _expr[n];
            }
        }

        void parse(int c)
        {
            (this->*_state)(c);
        }

        DtdState* finish()
        {
            parse( std::char_traits<Pt::Char>::eof() );

            reduceStack();

            if(_fragments.size() != 1)
                throw std::logic_error("DTD syntax error: incomplete expression");

            _fragments.top().patchLeafs(&_dtdEnd);
            DtdState* start = _fragments.top().start();
            _fragments.pop();
            return start;
        }

        DtdState* matched()
        {
            return _dtdEnd.matched();
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

            _ops.push(ch);
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
                _ops.push(ch);
                return;
            }

            if(ch == '*')
            {
                _ops.push(ch);
                return;
            }

            if(ch == '?')
            {
                _ops.push(ch);
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
                pushOperand();

                _ops.push(ch);
                _state = &DtdParser::OnExprBegin;
                return;
            }

            if( ch == '|')
            {
                pushOperand();

                _ops.push(ch);
                _state = &DtdParser::OnExprBegin;
                return;
            }

            if( ch == ')')
            {
                pushOperand();

                reduceStack();
                _state = &DtdParser::OnExprEnd;
                return;
            }

            throw std::logic_error("DTD syntax error: invalid identifier");
        }

        void pushOperand()
        {
            DtdElement* elem = new DtdElement(0, _token);
            _expr.push_back(elem);

            DtdFragment frag(elem);
            frag.setLeaf(elem);
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

                    DtdOr* dtdor = new DtdOr( op1.start(), op2.start() );
                    _expr.push_back(dtdor);

                    DtdFragment frag(dtdor);
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

                    DtdQuest* quest = new DtdQuest( op1.start() );
                    _expr.push_back(quest);
                    
                    DtdFragment frag( quest );
                    frag.setLeafs(op1.leafs(), quest);
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

                    DtdMult* mult = new DtdMult( op1.start() );
                    _expr.push_back(mult);

                    op1.patchLeafs(mult);
                    
                    DtdFragment frag( mult );
                    frag.setLeaf( mult );
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

                    DtdPlus* plus = new DtdPlus( op1.start() );
                    _expr.push_back(plus);

                    op1.patchLeafs(plus);
                    
                    DtdFragment frag( op1.start() );
                    frag.setLeaf( plus );
                    _fragments.push(frag);
                    continue;
                }
            }
        }

    private:
        typedef void (DtdParser::*ParseFunc)(int);
        ParseFunc _state;

        DtdEnd _dtdEnd;
        std::vector<DtdState*> _expr;

        Pt::String _token;
        std::stack<Pt::Char> _ops;
        std::stack<DtdFragment> _fragments;
};

// TODO:
class DtdElementDecl;
// owns states of state machine including DtdEnd
// name of element it declares

// TODO: 
class DtdElementValidator;
// pointer to DtdAttrListDecl
// pointer to DtdElementDecl
// validates attrs first, then uses state machine for child elemenets

void advance(std::vector<DtdState*>& current, std::vector<DtdState*>& next, Pt::Xml::Node& node)
{
    for(unsigned n = 0; n < current.size(); ++n)
    {
        DtdState* state = current[n];
        state->eval(node, next);
    }
            
    current = next;
    next.clear();
}


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
            DtdState* start = parser.finish();

            std::vector<DtdState*> current;
            std::vector<DtdState*> next;
            start->next(current);

            // first token <a>
            Pt::Xml::StartElement seA;
            seA.setName(L"a");
            advance(current, next, seA);

            // second token <b>
            Pt::Xml::StartElement seB;
            seB.setName(L"b");
            advance(current, next, seB);

            // third token <a>
            advance(current, next, seA);

            // fourth token <b>
            advance(current, next, seB);

            // 'end of input' token
            Pt::Xml::EndElement endElem;
            advance(current, next, endElem);

            bool isMatch = current.size() > 0 && current.at(0) == parser.matched();
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
