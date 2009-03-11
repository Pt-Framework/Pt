/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef Pt_XmlRpc_Args_h
#define Pt_XmlRpc_Args_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Parameter.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <string>

namespace Pt {

namespace XmlRpc {

class Args
{
    enum State
    {
        OnParams,
        OnParam
    };

    public:
        Args()
        : _state(OnParams)
        , _argNo(0)
        {}

        virtual ~Args()
        {}

        bool compose(const Xml::Node& node)
        {
            switch(_state)
            {
                case OnParams:
                {
                    if(node.type() == Xml::Node::StartElement)
                    {
                        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                        if(se.name() == L"param")
                        {
                            _state = OnParam;
                        }
                    }
                    else if(node.type() == Xml::Node::EndElement)
                    {
                        const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                        if(ee.name() == L"params")
                        {
                            return true;
                        }
                    }

                    break;
                }

                case OnParam:
                {
                    bool finished = composeParam(_argNo, node);
                    if(finished)
                    {
                        ++_argNo;
                        _state = OnParams;
                    }
                }
            }

            return false;
        }

    protected:
        virtual bool composeParam(unsigned n, const Xml::Node& node) = 0;

    private:
        State _state;
        unsigned _argNo;
};


template <typename A1, typename A2>
class BasicArgs : public Args
{
    public:
        bool composeParam(unsigned n, const Xml::Node& node)
        {
            switch(n)
            {
                case 0:
                    return _a1.compose(node);
                    break;

                case 1:
                    return _a2.compose(node);
                    break;
            }

            return true;
        }

        const A1& first() const
        { return _a1.get(); }

        const A2& second() const
        { return _a2.get(); }

    private:
        Parameter<A1> _a1;
        Parameter<A2> _a2;
};

}

}

#endif
