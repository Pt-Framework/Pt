/*
 * Copyright (C) 2004-2010 by Marc Boris Duerner
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
#ifndef PT_REFLEX_FUNCTIONPROXY_H
#define PT_REFLEX_FUNCTIONPROXY_H

#include <Pt/Reflex/FunctionInfo.h>
#include <Pt/Reflex/Type.h>
#include <Pt/Any.h>
#include <string>
#include <stdexcept>

namespace Pt {

namespace Reflex {

// template < typename R,
//            typename A1,
//            typename A2>
// class FunctionProxy : public FunctionInfo
// {
//     public:
//         typedef R (*FuncPtr)(A1, A2);

//     public:
//         FunctionProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
//         : _name(name)
//         , _proxy( proxy )
//         {
//             _rtype = ctx.getType( typeid(R) );
//             _a1type = ctx.getType( typeid(A1) );
//             _a2type = ctx.getType( typeid(A2) );
//         }

//         ~FunctionProxy()
//         { }

//         Any call(Any* args, Type** atypes, size_t argCount)
//         {
//             if(argCount != 2)
//                 throw std::invalid_argument("Not enough arguments");

//             A1 a1 = ArgumentTraits<A1>::cast( *_a1type, *atypes[0], args[0].get() );
//             A2 a2 = ArgumentTraits<A2>::cast( *_a2type, *atypes[1], args[1].get() );
//             R r = _proxy( a1, a2 );

//             return ReturnTraits<R>::make(r);
//         }

//         const char* name() const
//         { return _name.c_str(); }

//         size_t argSize() const
//         { return 2; }

//         Type& argType(size_t index) const
//         {
//             switch(index)
//             {
//                 case 0: return *_a1type;
//                 case 1: return *_a2type;
//             }

//             throw std::invalid_argument("No such argument");
//         }

//         Type& retType() const
//         { return *_rtype; }

//     private:
//         std::string _name;
//         FuncPtr _proxy;
//         Type* _rtype;
//         Type* _a1type;
//         Type* _a2type;
// };

/*
template < typename A1>
class FunctionProxy : public FunctionInfo
{
    public:
        typedef void (*FuncPtr)(A1);

    public:
        FunctionProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : _name(name)
        , _proxy( proxy )
        {
            _rtype = ctx.getType( typeid(void) );
            _a1type = ctx.getType( typeid(A1) );
        }

        Any call(Any* args, Type** atypes, size_t argCount)
        {
            if(argCount != 1)
                throw std::invalid_argument("Not enough arguments");

            A1 a1 = ArgumentTraits<A1>::cast( *_a1type, *atypes[0], args[0].get() );
            _proxy(a1);

            return Any();
        }

        const char* name() const
        { return _name.c_str(); }

        size_t argSize() const
        { return 1; }

        Type& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return *_a1type;
            }

            throw std::invalid_argument("No such argument");
        }

        Type& retType() const
        { return *_rtype; }

    private:
        std::string _name;
        FuncPtr _proxy;
        Type* _rtype;
        Type* _a1type;
};
*/

}

}

#endif
