/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Reflex_MethodInfoBase_h
#define Pt_Reflex_MethodInfoBase_h

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/CallableInfo.h>
#include <Pt/Exception.h>
#include <Pt/Void.h>


namespace Pt {

namespace Reflex {

template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void>
class MethodInfoBase : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 5; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
                case 2: return TypeTraits<A3>::typeName();
                case 3: return TypeTraits<A4>::typeName();
                case 4: return TypeTraits<A5>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
                case 3: return typeid(A4);
                case 4: return typeid(A5);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const char* retName() const
        { return TypeTraits<R>::typeName(); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfoBase<R, C, A1, A2, A3, A4, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 4; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
                case 2: return TypeTraits<A3>::typeName();
                case 3: return TypeTraits<A4>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
                case 3: return typeid(A4);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const char* retName() const
        { return TypeTraits<R>::typeName(); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfoBase<R, C, A1, A2, A3, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 3; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
                case 2: return TypeTraits<A3>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const char* retName() const
        { return TypeTraits<R>::typeName(); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class MethodInfoBase<R, C, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 2; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const char* retName() const
        { return TypeTraits<R>::typeName(); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C,
           typename A1>
class MethodInfoBase<R, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 1; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const char* retName() const
        { return TypeTraits<R>::typeName(); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C >
class MethodInfoBase<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 0; }

        const char* argName(size_t index) const
        { throw std::invalid_argument("No such argument" + PT_SOURCEINFO); }

        const std::type_info& argType(size_t index) const
        { throw std::invalid_argument("No such argument" + PT_SOURCEINFO); }

        const char* retName() const
        { return TypeTraits<R>::typeName(); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};

} // namespace Reflex

} // namespace Pt

#endif
