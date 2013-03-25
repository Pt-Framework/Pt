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
#include <Pt/Xml/XmlResolver.h>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Xml {

class InputStack
{
    public:
        struct Input
        {
            Input(InputSource* is, XmlResolver* res)
            : source(is)
            , resolver(res)
            {}

            InputSource* source;
            XmlResolver* resolver;
        };
    public:
        InputStack()
        : _externalDtd(0)
        , _currentInput(&_nullInput)
        {}

        ~InputStack()
        {
            clear();
        }

        void bumpLine()
        { _currentInput->setLine( _currentInput->line() + 1 ); }

        std::size_t line() const
        { 
            if( ! _external.empty() )
                return _external[0].source->line();

            return _currentInput->line(); 
        }

        bool empty() const
        { return _currentInput == &_nullInput; }

        void clear()
        {
            while( ! _external.empty() )
            {
                removeInput();
            }

            _currentInput = &_nullInput;
        }
                
        InputSource* current()
        { return _currentInput; }

        void addInput(InputSource& is, XmlResolver* resolver = 0)
        {
            _external.push_back( Input(&is, resolver) );
            _currentInput = &is;
        }

        bool isExternalDtd() const
        { return _externalDtd != 0; }

        void setExternalDtd(InputSource& is, XmlResolver* resolver)
        {
            addInput(is, resolver);
            _externalDtd = &is;
        }

        void removeInput()
        {
            _currentInput = &_nullInput;

            if( ! _external.empty() )
            {
                Input& in = _external.back();
                InputSource* is = in.source;
                XmlResolver* resolver = in.resolver;                        

                if( is == _externalDtd )
                    _externalDtd = 0;

                if(resolver)
                    resolver->releaseInput(is);

                _external.pop_back();
                        
                _currentInput = _external.empty() ? &_nullInput 
                                                  : _external.back().source;
            }
        }

    private:
        NullInputSource _nullInput;
        InputSource* _externalDtd;
        std::vector<Input> _external;
        InputSource* _currentInput;
};

} // namespace Xml

} // namespace Pt

#endif
