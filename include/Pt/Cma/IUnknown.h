/*
 * Copyright (C) 2006 by PTV AG
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
#ifndef PT_CMA_IUNKNOWN_H
#define PT_CMA_IUNKNOWN_H

#include <Pt/Cma/Api.h>
#include <Pt/Cma/TypeId.h>
#include <string>

namespace Pt {

namespace Cma {

/**
  *  Base interface for all services of a component.
  *
  *  This is the base interface for any service provided by a component.
  *  All services need to subclass this interface.
  */
class IUnknown
{
    template <typename IfaceT>
    friend IfaceT* queryInterface(IUnknown* unknown);

    public:
        /**
        * Creates a concrete interface type.
        *
        * Converts the initially received IUnknown interface type into a
        * concrete service interface provided by the component.
        *
        * @return pointer to an interface type
        */
        template <typename IfaceT>
        IfaceT* queryInterface()
        {
            return (IfaceT*) this->queryInterface( IfaceT::typeId() );
        }

        virtual IUnknown* queryInterface(const TypeId& typeId) = 0;

        /**
        * Decrement the reference count of this component.
        *
        * The components are reference counted and each time an interface
        * is queried, the reference counter is increased. If all interfaces
        * are released, the component is not referenced anymore and deletes
        * itself.
        */
        virtual void release() = 0;

    protected:
        //! Default destructor
        virtual ~IUnknown(void)
        {}
};

}

}

#endif
