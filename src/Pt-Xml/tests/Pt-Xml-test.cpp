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

class DtdState
{
    public:
        virtual ~DtdState()
        { }

        virtual void next(std::vector<DtdState*>& states) = 0;

        virtual void accept(Pt::Xml::Node& node, std::vector<DtdState*>& states) = 0;

        DtdState* out()
        { return _out; }

        DtdState** outPtr()
        { return &_out; }

        DtdState* out1()
        { return _out1; }

        DtdState** out1Ptr()
        { return &_out1; }

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

        virtual void accept(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }

        virtual void next(std::vector<DtdState*>& states) 
        {
            assert(out());
            out()->next(states);

            assert(out1());
            out1()->next(states);
        }
};


class DtdElement : public DtdState
{
    public:
        DtdElement(DtdState* next, const Pt::String& name)
        : DtdState(next, 0)
        , _name(name)
        { }

        virtual void accept(Pt::Xml::Node& node, std::vector<DtdState*>& states)
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

        virtual void accept(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        { }
        
        virtual void next(std::vector<DtdState*>& states) 
        { states.push_back(this); }
};


class DtdEnd : public DtdState
{
    public:
        DtdEnd()
        : DtdState()
        {
            setOut(&_end, 0);
        }

        virtual void accept(Pt::Xml::Node& node, std::vector<DtdState*>& states)
        {
            Pt::Xml::EndElement* e = Pt::Xml::toEndElement(&node);
            if(e != 0)
                out()->next(states);
        }
        
        virtual void next(std::vector<DtdState*>& states) 
        {
            states.push_back(this);
        }

        DtdState* matched()
        { return &_end; }

    private:
        DtdMatch _end;
};


class DtdFragment
{
    public:
        explicit DtdFragment(DtdState* state)
        : _state(state)
        {}

        DtdState* state()
        { return _state; }

        std::vector<DtdState**>& out()
        { return _out; }

        void appendOut(DtdState** next)
        { _out.push_back(next); }

        void setOut(std::vector<DtdState**>& states)
        { 
            _out = states; 
        }

        void patchOut(DtdState* to)
        {
            for(unsigned n = 0; n < _out.size(); ++n)
            {
                DtdState** s = _out[n];
                *s = to;
            }
        }

    private:
        DtdState* _state;
        std::vector<DtdState**> _out;
};


class DtdParser : private Pt::NonCopyable
{
    public:
        DtdParser()
        : _state(&DtdParser::OnBegin)
        , _depth(0)
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

            if(_stack.size() != 1)
                throw std::logic_error("DTD syntax error: incomplete expression");

            _stack.top().patchOut(&_dtdEnd);
            DtdState* start = _stack.top().state();
            _stack.pop();
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

            ++_depth;
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
            frag.appendOut( elem->outPtr() );
            _stack.push(frag);
        }

        void reduceStack()
        {
            for(;;)
            {
                if( _ops.empty() )
                    throw std::logic_error("internal DTD stack error");

                if(_ops.top() == '(')
                {
                    _ops.pop();
                    break;
                }

                if(_ops.top() == ',')
                {
                    _ops.pop();
                 
                    if( _stack.size() < 2 )
                        throw std::logic_error("DTD syntax error: no operands");
                    
                    DtdFragment op2 = _stack.top();
                    _stack.pop();

                    DtdFragment op1 = _stack.top();
                    _stack.pop();

                    op1.patchOut( op2.state() );
                    
                    DtdFragment frag( op1.state() );
                    frag.setOut( op2.out() );
                    _stack.push(frag);
                }
            }
        }

    private:
        typedef void (DtdParser::*ParseFunc)(int);
        ParseFunc _state;

        DtdEnd _dtdEnd;
        std::vector<DtdState*> _expr;

        unsigned _depth;
        Pt::String _token;
        std::stack<Pt::Char> _ops;
        std::stack<DtdFragment> _stack;
};

/*
    - push operators and open brackets on op stack
    - push literals directly on state stack
    - on closing bracket process operator stack backwards until open brace remove open brace
    - each processed operator removes its operands from state stack

    ((a, b) | (c, d))

    a b . c d . |

    OPS:    
    STATES: (ab) (cd) |
*/


class PtXmlTest : public Pt::Unit::TestSuite
{
    public:
        PtXmlTest()
        : Pt::Unit::TestSuite("Pt-Xml-Test")
        {
            this->registerMethod("DtdValidate", *this, &PtXmlTest::DtdValidate);
            this->registerMethod("DtdValidate2", *this, &PtXmlTest::DtdValidate2);
        }

    private:
        void DtdValidate2()
        {
            Pt::Xml::StartElement seA;
            seA.setName(L"a");
            
            Pt::Xml::StartElement seB;
            seB.setName(L"b");

            Pt::Xml::EndElement endElem;

            DtdParser parser;
            parser.parse('(');
            parser.parse('a');
            parser.parse(',');
            parser.parse('b');
            parser.parse(')');

            std::vector<DtdState*> _current;
            std::vector<DtdState*> _next;

            DtdState* start = parser.finish();
            start->next(_current);

            // first token <a>

            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->accept(seA, _next);
            }
            
            _current = _next;
            _next.clear();

            // first token <b>

            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->accept(seB, _next);
            }
            
            _current = _next;
            _next.clear();

            // end

            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->accept(endElem, _next);
            }

            _current = _next;
            _next.clear();

            bool isMatch = _current.size() > 0 && _current.at(0) == parser.matched();
            std::cout << isMatch << std::endl;
        }

        void DtdValidate()
        {
            Pt::Xml::StartElement seA;
            seA.setName(L"a");
            
            Pt::Xml::StartElement seB;
            seB.setName(L"b");

            Pt::Xml::EndElement endElem;

            DtdEnd end;
            
            /*
                (ab) | (cd)

                ab. cd. |
            */

            DtdElement childB(&end, L"b");
            DtdElement childA(&childB, L"a");

            DtdElement childD(&end, L"d");
            DtdElement childC(&childD, L"c");

            DtdOr orState(&childA, &childC);

            std::vector<DtdState*> _current;
            std::vector<DtdState*> _next;

            orState.next(_current);

            // first token <a>

            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->accept(seA, _next);
            }
            
            _current = _next;
            _next.clear();

            // first token <b>

            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->accept(seB, _next);
            }
            
            _current = _next;
            _next.clear();

            // end

            for(unsigned n = 0; n < _current.size(); ++n)
            {
                DtdState* state = _current[n];
                state->accept(endElem, _next);
            }

            _current = _next;
            _next.clear();

            bool isMatch = _current.size() > 0 && _current.at(0) == end.matched();
            std::cout << isMatch << std::endl;
        }
};

Pt::Unit::RegisterTest<PtXmlTest> register_PtXmlTest;
