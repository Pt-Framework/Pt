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
        typedef std::char_traits<Char>::int_type int_type;

        struct Input
        {
            Input(InputSource* is, XmlResolver* res)
            : source(is)
            , _avail(0)
            , resolver(res)
            {}

            inline std::streamsize avail()
            { return _avail; }

            inline std::streamsize import()
            {
                _avail = source->import();
                return _avail;
            }

            inline int_type get()
            {
                if(_avail > 0)
                    --_avail;
                return source->get();
            }

            InputSource* source;
            std::streamsize _avail;
            XmlResolver* resolver;
        };

    public:
        InputStack()
        : _externalDtd(0)
        , _currentInput(0)
        {
            _sources.push_back( Input(&_nullInput, 0) );
            _currentInput = &_sources.back();
        }

        ~InputStack()
        {
            clear();
        }
        
        inline Input& input() 
        { return *_currentInput; }

        inline void bumpLine()
        { _currentInput->source->setLine( line() + 1 ); }

        inline std::size_t line() const
        { return _currentInput->source->line(); }

        inline bool empty() const
        { return _sources.size() <= 1; }

        std::size_t size() const
        { return _sources.size() -1; }

        inline void clear()
        {
            while( ! empty() )
                removeInput();
        }

        inline int_type get()
        { return _currentInput->get(); }
               
        inline std::streamsize import()
        { return _currentInput->import(); }

        inline std::streamsize avail()
        { return _currentInput->avail(); }

        inline InputSource* source()
        { return _currentInput->source; }

        void addInput(InputSource& is, XmlResolver* resolver = 0)
        {
            _sources.push_back( Input(&is, resolver) );
            _currentInput = &_sources.back();
        }

        inline bool isExternalDtd() const
        { return _externalDtd != 0; }

        void setExternalDtd(InputSource& is, XmlResolver* resolver)
        {
            addInput(is, resolver);
            _externalDtd = &is;
        }

        void removeInput()
        {
            if( ! empty() )
            {
                Input& in = _sources.back();
                InputSource* is = in.source;
                XmlResolver* resolver = in.resolver;                        

                if( is == _externalDtd )
                    _externalDtd = 0;

                if(resolver)
                    resolver->releaseInput(is);

                _sources.pop_back();
                _currentInput = & _sources.back();
            }
        }

    private:
        NullInputSource _nullInput;
        InputSource* _externalDtd;
        std::vector<Input> _sources;
        Input* _currentInput;
};

} // namespace Xml

} // namespace Pt

#endif
