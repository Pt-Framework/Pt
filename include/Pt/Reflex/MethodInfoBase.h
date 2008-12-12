/*
 * Copyright (C) 2005 by Marc Boris Duerner
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
#ifndef Pt_Reflex_MethodInfoBase_h
#define Pt_Reflex_MethodInfoBase_h

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/CallableInfo.h>
#include <Pt/SourceInfo.h>
#include <Pt/Void.h>
#include <stdexcept>

namespace Pt {

namespace Reflex {

template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void,
           typename A6 = Pt::Void,
           typename A7 = Pt::Void,
           typename A8 = Pt::Void>
class MethodInfoBase : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 8; }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
                case 3: return typeid(A4);
                case 4: return typeid(A5);
                case 5: return typeid(A6);
                case 6: return typeid(A7);
                case 7: return typeid(A8);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

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
           typename A4,
           typename A5>
class MethodInfoBase<R, C, A1, A2, A3, A4, A5, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 5; }

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
class MethodInfoBase<R, C, A1, A2, A3, A4, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 4; }

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
class MethodInfoBase<R, C, A1, A2, A3, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 3; }

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

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class MethodInfoBase<R, C, A1, A2, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 2; }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C,
           typename A1>
class MethodInfoBase<R, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 1; }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};


template < typename R,
           class C >
class MethodInfoBase<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public CallableInfo
{
    public:
        MethodInfoBase(const std::string& name)
        : _name(name)
        {}

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 0; }

        const std::type_info& argType(size_t index) const
        { throw std::invalid_argument("No such argument" + PT_SOURCEINFO); }

        const std::type_info& retType() const
        { return typeid(R); }

    private:
        std::string _name;
};

} // namespace Reflex

} // namespace Pt

#endif
