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
#ifndef Pt_XmlRpc_Service_h
#define Pt_XmlRpc_Service_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/Http/Service.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>
#include <Pt/Void.h>
#include <Pt/TypeTraits.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class ServiceProcedure
{
    public:
        ServiceProcedure()
        {}

        virtual ~ServiceProcedure()
        {}

        virtual ServiceProcedure* clone(SerializationContext* ctx) const = 0;

        virtual IComposer** beginCall() = 0;

        virtual IDecomposer* endCall() = 0;
};


}

}

#include <Pt/XmlRpc/Service.tpp>

#endif
