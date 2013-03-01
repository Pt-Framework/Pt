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

#ifndef Pt_Xml_InputStack_h
#define Pt_Xml_InputStack_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/InputSource.h>
#include <stack>
#include <cstddef>

namespace Pt {

namespace Xml {

class InputStack
{
    public:
        InputStack()
        : _input(&_nullInput)
        , _externalDtd(0)
        , _currentInput(&_nullInput)
        {}

        ~InputStack()
        {
            clear();
        }

        void bumpLine()
        { _currentInput->setLine( _currentInput->line() + 1 ); }

        std::size_t line() const
        { return _currentInput->line(); }

        bool empty() const
        { return _currentInput == &_nullInput; }

        bool isPrimary() const
        { return _currentInput == _input; }

        void clear()
        {
            while( ! _external.empty() )
            {
                removeInput();
            }

            _currentInput = &_nullInput;
            _input = &_nullInput;
        }
                
        InputSource* currentInput()
        { return _currentInput; }

        void setInput(InputSource& is)
        {
            _input = &is;

            if( _external.empty() )
            {
                _currentInput = &is;
            }
        }

        void addInput(InputSource* is)
        {
            std::auto_ptr<InputSource> isPtr;
            if(is->refs() == 0)
                isPtr.reset(is);

            _external.push(is);
            isPtr.release();

            _currentInput = is;
        }

        void setExternalDtd(InputSource* is)
        {
            addInput(is);
            _externalDtd = is;
        }

        InputSource* externalDtd()
        { return _externalDtd; }

        bool isExternalDtd() const
        { return _externalDtd != 0; }

        void removeInput()
        {
            _currentInput = &_nullInput;

            if( ! _external.empty() )
            {
                InputSource* is = _external.top();                        

                if( is == _externalDtd ) 
                    _externalDtd = 0;

                if( is->refs() == 0 )
                    delete _external.top();

                _external.pop();
                        
                _currentInput = _external.empty() ? _input 
                                                  : _external.top();
            }
        }

    private:
        NullInputSource _nullInput;
        InputSource* _input;
        InputSource* _externalDtd;
        std::stack<InputSource*> _external;
        InputSource* _currentInput;
};

} // namespace Xml

} // namespace Pt

#endif
